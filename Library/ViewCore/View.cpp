/* This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt */

#include "View.h"
#include "renderer.h"



View::View(Surface* surface) :
_surface(surface),
_viewport(),
_aspect(0),
_flip(false)
{
	UpdateAspect();
}


View::~View()
{
}


bounds2f View::GetViewportBounds() const
{
	return _viewport.is_empty() ? bounds2f(0, 0, _surface->GetSize()) : _viewport;
}


void View::SetViewport(bounds2f value)
{
	_viewport = value;
	UpdateAspect();
}


void View::UseViewport()
{
	if (_viewport.is_empty())
	{
		_surface->UseViewport();
	}
	else
	{
		bounds2f viewport = _viewport * GetScreen()->GetPixelDensity();
		glViewport((GLint)viewport.min.x, (GLint)viewport.min.y, (GLsizei)viewport.size().x, (GLsizei)viewport.size().y);
	}
}


glm::vec2 View::ScreenToView(glm::vec2 value) const
{
	bounds2f viewport = GetViewportBounds();
	return 2.0f * (value - viewport.p11()) / viewport.size() - 1.0f;
}


glm::vec2 View::ViewToScreen(glm::vec2 value) const
{
	bounds2f viewport = GetViewportBounds();
	return viewport.p11() + (value + 1.0f) / 2.0f * viewport.size();
}


void View::ScreenSizeChanged()
{
	UpdateAspect();
}


/*void View::Update(double secondsSinceLastUpdate)
{
}*/



void View::UpdateAspect()
{
	if (_viewport.is_empty())
		_aspect = _surface->GetSize().y / _surface->GetSize().x;
	else
		_aspect = _viewport.size().y / _viewport.size().x;
}
