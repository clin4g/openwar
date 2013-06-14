// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "TiledTerrainRenderer.h"
#include "renderer.h"
#include "image.h"
#include "TiledTerrainModel.h"


TiledTerrainRenderer::TiledTerrainRenderer(TiledTerrainModel* terrainModel) :
_terrainModel(terrainModel)
{
}


TiledTerrainRenderer::~TiledTerrainRenderer()
{
}


void TiledTerrainRenderer::Render(const glm::mat4x4& transform, const glm::vec3& lightNormal)
{
	bounds2f bounds = _terrainModel->GetBounds();
	glm::ivec2 size = _terrainModel->GetSize();

	shape<texture_vertex3> shape;
	shape._mode = GL_TRIANGLES;

	glm::vec2 delta = bounds.size() / glm::vec2(size);

	for (int x = 0; x < size.x; ++x)
		for (int y = 0; y < size.y; ++y)
		{
			TiledTerrainModel::Tile* tile = _terrainModel->GetTile(x, y);

			glm::vec2 p0 = bounds.min + delta * glm::vec2(x, y);
			glm::vec2 p1 = p0 + delta;

			float h00 = _terrainModel->GetHeight(glm::vec2(p0.x, p0.y));
			float h01 = _terrainModel->GetHeight(glm::vec2(p0.x, p1.y));
			float h10 = _terrainModel->GetHeight(glm::vec2(p1.x, p0.y));
			float h11 = _terrainModel->GetHeight(glm::vec2(p1.x, p1.y));


			glm::vec2 t00 = glm::vec2(0, 0);
			glm::vec2 t01 = glm::vec2(0, 1);
			glm::vec2 t10 = glm::vec2(1, 0);
			glm::vec2 t11 = glm::vec2(1, 1);

			for (int i = 0; i < 3; ++i)
			{
				glm::vec2 tmp = t00;
				t00 = t01;
				t01 = t11;
				t11 = t10;
				t10 = tmp;
			}

			if (tile->mirror)
			{
				t00.x = 1 - t00.x;
				t01.x = 1 - t01.x;
				t10.x = 1 - t10.x;
				t11.x = 1 - t11.x;
			}

			for (int i = 0; i < tile->rotate; ++i)
			{
				glm::vec2 tmp = t00;
				t00 = t01;
				t01 = t11;
				t11 = t10;
				t10 = tmp;
			}

			shape._vertices.clear();
			shape._vertices.push_back(texture_vertex3(glm::vec3(p0.x, p0.y, h00), t01));
			shape._vertices.push_back(texture_vertex3(glm::vec3(p1.x, p0.y, h10), t11));
			shape._vertices.push_back(texture_vertex3(glm::vec3(p1.x, p1.y, h11), t10));
			shape._vertices.push_back(texture_vertex3(glm::vec3(p1.x, p1.y, h11), t10));
			shape._vertices.push_back(texture_vertex3(glm::vec3(p0.x, p1.y, h01), t00));
			shape._vertices.push_back(texture_vertex3(glm::vec3(p0.x, p0.y, h00), t01));

			shape.update(GL_STATIC_DRAW);

			texture_uniforms uniforms;
			uniforms._transform = transform;
			uniforms._texture = tile->texture;

			renderers::singleton->_texture_renderer3->render(shape, uniforms);
		}
}
