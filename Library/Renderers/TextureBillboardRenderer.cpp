// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TextureBillboardRenderer.h"


TextureBillboardRenderer::TextureBillboardRenderer()
{
	_texture_billboard_renderer = new renderer<texture_billboard_vertex, texture_billboard_uniforms>((
		VERTEX_ATTRIBUTE(texture_billboard_vertex, _position),
		VERTEX_ATTRIBUTE(texture_billboard_vertex, _height),
		VERTEX_ATTRIBUTE(texture_billboard_vertex, _texcoord),
		VERTEX_ATTRIBUTE(texture_billboard_vertex, _texsize),
		SHADER_UNIFORM(texture_billboard_uniforms, _transform),
		SHADER_UNIFORM(texture_billboard_uniforms, _texture),
		SHADER_UNIFORM(texture_billboard_uniforms, _upvector),
		SHADER_UNIFORM(texture_billboard_uniforms, _viewport_height),
		SHADER_UNIFORM(texture_billboard_uniforms, _min_point_size),
		SHADER_UNIFORM(texture_billboard_uniforms, _max_point_size),
		VERTEX_SHADER
		({
			uniform mat4 transform;
			uniform vec3 upvector;
			uniform float viewport_height;
			uniform float min_point_size;
			uniform float max_point_size;
			attribute vec3 position;
			attribute float height;
			attribute vec2 texcoord;
			attribute vec2 texsize;
			varying vec2 _texcoord;
			varying vec2 _texsize;

		void main()
		{
			vec3 position2 = position + height * 0.5 * viewport_height * upvector;
			vec4 p = transform * vec4(position, 1);
			vec4 q = transform * vec4(position2, 1);
			float s = clamp(abs(q.y / q.w - p.y / p.w), min_point_size, max_point_size);

			_texcoord = texcoord;
			_texsize = texsize;

			gl_Position = p;
			gl_PointSize = s;
		}
	}),
	FRAGMENT_SHADER
	({
		uniform sampler2D texture;
		varying vec2 _texcoord;
		varying vec2 _texsize;

		void main()
		{
			vec4 color = texture2D(texture, _texcoord + gl_PointCoord * _texsize);

			gl_FragColor = color;
		}
	})));
	_texture_billboard_renderer->_blend_sfactor = GL_ONE;
	_texture_billboard_renderer->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


TextureBillboardRenderer::~TextureBillboardRenderer()
{
}


void TextureBillboardRenderer::Reset()
{
	_vbo._mode = GL_POINTS;
	_vbo._vertices.clear();
}


void TextureBillboardRenderer::AddBillboard(glm::vec3 position, float height, affine2 texcoords)
{
	glm::vec2 texpos = texcoords.transform(glm::vec2(0, 0));
	glm::vec2 texsize = texcoords.transform(glm::vec2(1, 1)) - texpos;

	_vbo._vertices.push_back(texture_billboard_vertex(position, height, texpos, texsize));
}


void TextureBillboardRenderer::Draw(texture* tex, const glm::mat4x4& transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight, bounds1f sizeLimit)
{
	float a = -glm::radians(cameraFacingDegrees);
	float cos_a = cosf(a);
	float sin_a = sinf(a);
	for (texture_billboard_vertex& v : _vbo._vertices)
		v._order = cos_a * v._position.x - sin_a * v._position.y;

	std::sort(_vbo._vertices.begin(), _vbo._vertices.end(), [](const texture_billboard_vertex& a, const texture_billboard_vertex& b) {
		return a._order > b._order;
	});
	_vbo.update(GL_STATIC_DRAW);

	texture_billboard_uniforms uniforms;
	uniforms._transform = transform;
	uniforms._texture = tex;
	uniforms._upvector = cameraUp;
	uniforms._viewport_height = renderer_base::pixels_per_point() * viewportHeight;
	uniforms._min_point_size = sizeLimit.min;
	uniforms._max_point_size = sizeLimit.max;

	_texture_billboard_renderer->render(_vbo, uniforms);
}


void TextureBillboardRenderer::Render(BillboardModel* billboardModel, glm::mat4x4 const & transform, const glm::vec3& cameraUp, float cameraFacingDegrees, float viewportHeight)
{
	Reset();

	for (const Billboard& billboard : billboardModel->staticBillboards)
		AddBillboard(billboard.position, billboard.height, billboardModel->texture->GetTexCoords(billboard.shape, billboard.facing - cameraFacingDegrees));

	for (const Billboard& billboard : billboardModel->dynamicBillboards)
		AddBillboard(billboard.position, billboard.height, billboardModel->texture->GetTexCoords(billboard.shape, billboard.facing - cameraFacingDegrees));

	Draw(billboardModel->texture->GetTexture(), transform, cameraUp, cameraFacingDegrees, viewportHeight);
}
