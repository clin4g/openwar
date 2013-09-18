// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ButtonRendering_H
#define ButtonRendering_H

#include "bounds.h"
#include "texture.h"
#include "string_shape.h"


struct ButtonIcon;

struct ButtonRendering
{
	renderers* _renderers;
	string_font* _string_font;
	string_shape* _string_shape;

	texture* _textureButtonBackground;
	texture* _textureButtonHighlight;
	texture* _textureButtonSelected;
	texture* _textureButtonIcons;
	texture* _textureEditorTools;

	ButtonIcon* buttonIconPlay;
	ButtonIcon* buttonIconPause;
	ButtonIcon* buttonIconRewind;
	ButtonIcon* buttonIconHelp;
	ButtonIcon* buttonIconChat;

	ButtonIcon* buttonEditorToolHand;
	ButtonIcon* buttonEditorToolPaint;
	ButtonIcon* buttonEditorToolErase;
	ButtonIcon* buttonEditorToolSmear;
	ButtonIcon* buttonEditorToolHills;
	ButtonIcon* buttonEditorToolTrees;
	ButtonIcon* buttonEditorToolWater;
	ButtonIcon* buttonEditorToolFords;

	ButtonRendering(renderers* r, float pixelDensity);

	void RenderBackground(bounds2f viewport, bounds2f bounds);
	void RenderHighlight(bounds2f viewport, bounds2f bounds);
	void RenderSelected(bounds2f viewport, bounds2f bounds);
	void RenderButtonText(bounds2f viewport, glm::vec2 position, const char* text);

	void RenderCornerButton(bounds2f viewport, texture* texture, bounds2f bounds, float radius);
	void RenderButtonIcon(bounds2f viewport, glm::vec2 position, ButtonIcon* buttonIcon, bool disabled);
	void RenderTextureRect(bounds2f viewport, texture* texture, bounds2f b, bounds2f t, float alpha = 1);

};


#endif
