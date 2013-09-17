// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ButtonRendering.h"
#include "ButtonView.h"
#include "vertexbuffer.h"
#include "sprite.h"



ButtonRendering::ButtonRendering(renderers* r, float pixelDensity) :
_renderers(r)
{
	_textureButtonBackground = new texture("ButtonNormal.png");
	_textureButtonHighlight = new texture("ButtonHighlight.png");
	_textureButtonSelected = new texture("ButtonSelected.png");
	_textureButtonIcons = new texture("ButtonIcons.png");

	_string_font = new string_font(true, 18, pixelDensity);
	_string_shape = new string_shape(_string_font);

	_textureEditorTools = new texture("EditorTools.png");

	buttonIconPlay = new ButtonIcon(_textureButtonIcons, glm::vec2(25, 32), bounds2f(0, 0, 25, 32) / glm::vec2(128, 32));
	buttonIconPause = new ButtonIcon(_textureButtonIcons, glm::vec2(25, 32), bounds2f(25, 0, 50, 32) / glm::vec2(128, 32));
	buttonIconRewind = new ButtonIcon(_textureButtonIcons, glm::vec2(25, 32), bounds2f(50, 0, 75, 32) / glm::vec2(128, 32));
	buttonIconHelp = new ButtonIcon(_textureButtonIcons, glm::vec2(25, 32), bounds2f(75, 0, 100, 32) / glm::vec2(128, 32));
	buttonIconChat = new ButtonIcon(_textureButtonIcons, glm::vec2(25, 32), bounds2f(100, 0, 125, 32) / glm::vec2(128, 32));

	buttonEditorToolHand = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.00, 0.0, 0.25, 0.5));
	buttonEditorToolPaint = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.25, 0.0, 0.50, 0.5));
	buttonEditorToolErase = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.50, 0.0, 0.75, 0.5));
	buttonEditorToolSmear = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.75, 0.0, 1.00, 0.5));
	buttonEditorToolHills = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.00, 0.5, 0.25, 1.0));
	buttonEditorToolTrees = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.25, 0.5, 0.50, 1.0));
	buttonEditorToolWater = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.50, 0.5, 0.75, 1.0));
	buttonEditorToolFords = new ButtonIcon(_textureEditorTools, glm::vec2(32, 32), bounds2f(0.75, 0.5, 1.00, 1.0));
}


static void AddRect(vertexbuffer<texture_vertex>& shape, bounds2f bounds, bounds2f texture)
{
	shape._vertices.push_back(texture_vertex(bounds.p11(), glm::vec2(0, 1) - texture.p11()));
	shape._vertices.push_back(texture_vertex(bounds.p12(), glm::vec2(0, 1) - texture.p12()));
	shape._vertices.push_back(texture_vertex(bounds.p22(), glm::vec2(0, 1) - texture.p22()));

	shape._vertices.push_back(texture_vertex(bounds.p22(), glm::vec2(0, 1) - texture.p22()));
	shape._vertices.push_back(texture_vertex(bounds.p21(), glm::vec2(0, 1) - texture.p21()));
	shape._vertices.push_back(texture_vertex(bounds.p11(), glm::vec2(0, 1) - texture.p11()));
}


void ButtonRendering::RenderCornerButton(bounds2f viewport, texture* texture, bounds2f bounds, float radius)
{
	vertexbuffer<texture_vertex> shape;
	shape._mode = GL_TRIANGLES;

	bounds2f outer = bounds;
	bounds2f inner = outer.grow(-radius);

	AddRect(shape, bounds2f(outer.min.x, outer.min.y, inner.min.x, inner.min.y), bounds2f(0.0, 0.0, 0.5, 0.5));
	AddRect(shape, bounds2f(inner.min.x, outer.min.y, inner.max.x, inner.min.y), bounds2f(0.5, 0.0, 0.5, 0.5));
	AddRect(shape, bounds2f(inner.max.x, outer.min.y, outer.max.x, inner.min.y), bounds2f(0.5, 0.0, 1.0, 0.5));

	AddRect(shape, bounds2f(outer.min.x, inner.min.y, inner.min.x, inner.max.y), bounds2f(0.0, 0.5, 0.5, 0.5));
	AddRect(shape, bounds2f(inner.min.x, inner.min.y, inner.max.x, inner.max.y), bounds2f(0.5, 0.5, 0.5, 0.5));
	AddRect(shape, bounds2f(inner.max.x, inner.min.y, outer.max.x, inner.max.y), bounds2f(0.5, 0.5, 1.0, 0.5));

	AddRect(shape, bounds2f(outer.min.x, inner.max.y, inner.min.x, outer.max.y), bounds2f(0.0, 0.5, 0.5, 1.0));
	AddRect(shape, bounds2f(inner.min.x, inner.max.y, inner.max.x, outer.max.y), bounds2f(0.5, 0.5, 0.5, 1.0));
	AddRect(shape, bounds2f(inner.max.x, inner.max.y, outer.max.x, outer.max.y), bounds2f(0.5, 0.5, 1.0, 1.0));

	texture_uniforms uniforms;
	uniforms._transform = sprite_transform(viewport).transform();
	uniforms._texture = texture;

	_renderers->_texture_renderer->render(shape, uniforms);
}


void ButtonRendering::RenderButtonIcon(bounds2f viewport, glm::vec2 position, ButtonIcon* buttonIcon, bool disabled)
{
	if (buttonIcon != nullptr)
	{
		bounds2f b = bounds2f(position).grow(buttonIcon->size * buttonIcon->scale / 2.0f);
		RenderTextureRect(viewport, buttonIcon->_texture, b, buttonIcon->bounds, disabled ? 0.5 : 1);
	}
}


void ButtonRendering::RenderTextureRect(bounds2f viewport, texture* texture, bounds2f b, bounds2f t, float alpha)
{
	vertexbuffer<texture_vertex> shape;
	shape._mode = GL_TRIANGLE_STRIP;
	shape._vertices.push_back(texture_vertex(b.p11(), t.p12()));
	shape._vertices.push_back(texture_vertex(b.p12(), t.p11()));
	shape._vertices.push_back(texture_vertex(b.p21(), t.p22()));
	shape._vertices.push_back(texture_vertex(b.p22(), t.p21()));

	if (alpha == 1)
	{
		texture_uniforms uniforms;
		uniforms._transform = sprite_transform(viewport).transform();
		uniforms._texture = texture;

		_renderers->_texture_renderer->render(shape, uniforms);
	}
	else
	{
		texture_alpha_uniforms uniforms;
		uniforms._transform = sprite_transform(viewport).transform();
		uniforms._texture = texture;
		uniforms._alpha = alpha;

		_renderers->_alpha_texture_renderer->render(shape, uniforms);
	}
}


void ButtonRendering::RenderBackground(bounds2f viewport, bounds2f bounds)
{
	RenderCornerButton(viewport, _textureButtonBackground, bounds.grow(10), 32);
}


void ButtonRendering::RenderHighlight(bounds2f viewport, bounds2f bounds)
{
	RenderTextureRect(viewport, _textureButtonHighlight, bounds, bounds2f(0, 0, 1, 1));
	//RenderCornerButton(viewport, _texture_highlight, bounds.grow(10), 32);
}


void ButtonRendering::RenderSelected(bounds2f viewport, bounds2f bounds)
{
	//RenderTextureRect(viewport, _texture_selected, bounds, bounds2(0, 0, 1, 1));
	RenderCornerButton(viewport, _textureButtonSelected, bounds.grow(10), 32);
}


void ButtonRendering::RenderButtonText(bounds2f viewport, glm::vec2 position, NSString* text)
{
	_string_shape->clear();
	_string_shape->add(text, glm::mat4x4());
	_string_shape->update(GL_STATIC_DRAW);

	string_sprite sprite(_string_font->_renderer);
	sprite._viewport = viewport;
	sprite._texture = &_string_font->_texture;
	sprite._shape = &_string_shape->_vbo;

	glm::vec2 p = position - 0.5f * _string_font->measure(text) - glm::vec2(0, 1);

	sprite._color = glm::vec4(0, 0, 0, 0.15);
	for (int dx = -1; dx <= 1; ++dx)
		for (int dy = -1; dy <= 1; ++dy)
			if (dx != 0 || dy != 0)
			{
				sprite._translate = p + glm::vec2(dx, dy);
				sprite.render();
			}

	sprite._translate = p + glm::vec2(0, -1);
	sprite._color = glm::vec4(0, 0, 0, 1);
	sprite.render();

	sprite._translate = p;
	sprite._color = glm::vec4(1, 1, 1, 1);
	sprite.render();
}
