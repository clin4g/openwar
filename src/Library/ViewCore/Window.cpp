// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <ctime>
#include <GL/glew.h>
#include <GL/gl.h>
//#include <OpenGL/gl.h>

#include "Window.h"
#include "Surface.h"
#include "Gesture.h"


bool Window::_done = false;
std::map<Uint32, Window*> Window::_windows;


static double current_timestamp()
{
	return (double)std::clock() / CLOCKS_PER_SEC;
}



Window::Window() :
_surface(nullptr),
_window(nullptr),
_glcontext(0),
_touch(nullptr),
_timestamp(0)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

	_window = SDL_CreateWindow("My Game Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1024, 768,
		/*SDL_WINDOW_FULLSCREEN |*/ SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	_glcontext = SDL_GL_CreateContext(_window);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);

	Uint32 windowID = SDL_GetWindowID(_window);

	_windows[windowID] = this;

	_timestamp = current_timestamp();
}


Window::~Window()
{
}


bool Window::IsDone()
{
	return _done;
}


void Window::ProcessEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ProcessEvent(event);
	}

	for (auto i : _windows)
	{
		i.second->Update();
		i.second->Render();
	}
}


Window* Window::GetWindow(Uint32 windowID)
{
	return windowID != 0 ? _windows[windowID] : nullptr;
}



/*static NSString* EventTypeToString(Uint32 type)
{
	switch (type)
	{
		case SDL_QUIT: return @"SDL_QUIT";
		case SDL_APP_TERMINATING: return @"SDL_APP_TERMINATING";
		case SDL_APP_LOWMEMORY: return @"SDL_APP_LOWMEMORY";
		case SDL_APP_WILLENTERBACKGROUND: return @"SDL_APP_WILLENTERBACKGROUND";
		case SDL_APP_DIDENTERBACKGROUND: return @"SDL_APP_DIDENTERBACKGROUND";
		case SDL_APP_WILLENTERFOREGROUND: return @"SDL_APP_WILLENTERFOREGROUND";
		case SDL_APP_DIDENTERFOREGROUND: return @"SDL_APP_DIDENTERFOREGROUND";
		case SDL_WINDOWEVENT: return @"SDL_WINDOWEVENT";
		case SDL_SYSWMEVENT: return @"SDL_SYSWMEVENT";
		case SDL_KEYDOWN: return @"SDL_KEYDOWN";
		case SDL_KEYUP: return @"SDL_KEYUP";
		case SDL_TEXTEDITING: return @"SDL_TEXTEDITING";
		case SDL_TEXTINPUT: return @"SDL_TEXTINPUT";
		case SDL_MOUSEMOTION: return @"SDL_MOUSEMOTION";
		case SDL_MOUSEBUTTONDOWN: return @"SDL_MOUSEBUTTONDOWN";
		case SDL_MOUSEBUTTONUP: return @"SDL_MOUSEBUTTONUP";
		case SDL_MOUSEWHEEL: return @"SDL_MOUSEWHEEL";
		case SDL_JOYAXISMOTION: return @"SDL_JOYAXISMOTION";
		case SDL_JOYBALLMOTION: return @"SDL_JOYBALLMOTION";
		case SDL_JOYHATMOTION: return @"SDL_JOYHATMOTION";
		case SDL_JOYBUTTONDOWN: return @"SDL_JOYBUTTONDOWN";
		case SDL_JOYBUTTONUP: return @"SDL_JOYBUTTONUP";
		case SDL_JOYDEVICEADDED: return @"SDL_JOYDEVICEADDED";
		case SDL_JOYDEVICEREMOVED: return @"SDL_JOYDEVICEREMOVED";
		case SDL_CONTROLLERAXISMOTION: return @"SDL_CONTROLLERAXISMOTION";
		case SDL_CONTROLLERBUTTONDOWN: return @"SDL_CONTROLLERBUTTONDOWN";
		case SDL_CONTROLLERBUTTONUP: return @"SDL_CONTROLLERBUTTONUP";
		case SDL_CONTROLLERDEVICEADDED: return @"SDL_CONTROLLERDEVICEADDED";
		case SDL_CONTROLLERDEVICEREMOVED: return @"SDL_CONTROLLERDEVICEREMOVED";
		case SDL_CONTROLLERDEVICEREMAPPED: return @"SDL_CONTROLLERDEVICEREMAPPED";
		case SDL_FINGERDOWN: return @"SDL_FINGERDOWN";
		case SDL_FINGERUP: return @"SDL_FINGERUP";
		case SDL_FINGERMOTION: return @"SDL_FINGERMOTION";
		case SDL_DOLLARGESTURE: return @"SDL_DOLLARGESTURE";
		case SDL_DOLLARRECORD: return @"SDL_DOLLARRECORD";
		case SDL_MULTIGESTURE: return @"SDL_MULTIGESTURE";
		case SDL_CLIPBOARDUPDATE: return @"SDL_CLIPBOARDUPDATE";
		case SDL_DROPFILE: return @"SDL_DROPFILE";
		case SDL_USEREVENT: return @"SDL_USEREVENT";
		default: return @"[event?]";
	}
}*/



void Window::ProcessEvent(const SDL_Event& event)
{
	//NSLog(@"ProcessEvent %@", EventTypeToString(event.type));

	Window* window;
	switch (event.type)
	{
		case SDL_QUIT:
			_done = true;
			break;

		case SDL_WINDOWEVENT:
			window = GetWindow(event.window.windowID);
			if (window != nullptr) window->ProcessWindow(event.window);
			break;

		case SDL_KEYDOWN:
			window = GetWindow(event.key.windowID);
			if (window != nullptr) window->ProcessKeyDown(event.key);
			break;

		case SDL_KEYUP:
			window = GetWindow(event.key.windowID);
			if (window != nullptr) window->ProcessKeyUp(event.key);
			break;

		case SDL_MOUSEMOTION:
			window = GetWindow(event.motion.windowID);
			if (window != nullptr) window->ProcessMouseMotion(event.motion);
			break;

		case SDL_MOUSEBUTTONDOWN:
			window = GetWindow(event.button.windowID);
			if (window != nullptr) window->ProcessMouseButtonDown(event.button);
			break;

		case SDL_MOUSEBUTTONUP:
			window = GetWindow(event.button.windowID);
			if (window != nullptr) window->ProcessMouseButtonUp(event.button);
			break;

		case SDL_MOUSEWHEEL:
			window = GetWindow(event.wheel.windowID);
			if (window != nullptr) window->ProcessMouseWheel(event.wheel);
			break;
	}
}


void Window::ProcessWindow(const SDL_WindowEvent& event)
{
	int x, y, w, h;
	SDL_GetWindowPosition(_window, &x, &y);
	SDL_GetWindowSize(_window, &w, &h);

	_surface->SetOrigin(glm::vec2(x, y));
	_surface->SetSize(glm::vec2(w, h));
	_surface->ScreenSizeChanged();
}


static char TranslateKeyCode(const SDL_Keysym& keysym)
{
	switch (keysym.sym)
	{
		case SDLK_q: return 'Q';
		case SDLK_w: return 'W';
		case SDLK_e: return 'E';
		case SDLK_a: return 'A';
		case SDLK_s: return 'S';
		case SDLK_d: return 'D';
		case SDLK_1: return '1';
		case SDLK_2: return '2';
		case SDLK_3: return '3';
		case SDLK_4: return '4';
		case SDLK_5: return '5';
		case SDLK_6: return '6';
		case SDLK_7: return '7';
		case SDLK_8: return '8';
		case SDLK_9: return '9';
		default: return '\0';
	}
}


void Window::ProcessKeyDown(const SDL_KeyboardEvent& event)
{
	char key = TranslateKeyCode(event.keysym);
	if (key == '\0')
		return;

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->KeyDown(_surface, key);

}


void Window::ProcessKeyUp(const SDL_KeyboardEvent& event)
{
	char key = TranslateKeyCode(event.keysym);
	if (key == '\0')
		return;

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->KeyUp(_surface, key);
}


void Window::ProcessMouseMotion(const SDL_MouseMotionEvent& event)
{
	//NSLog(@"ProcessMouseMotion which=%d state=%d", event.which, event.state);

	if (_touch != nullptr)
	{
		glm::vec2 position = ToVector(event.x, event.y);
		double timestamp = ToTimestamp(event.timestamp);

		MouseButtons buttons = _touch->GetCurrentButtons();
		buttons.left = (event.state & SDL_BUTTON_LMASK) != 0;
		buttons.right = (event.state & SDL_BUTTON_RMASK) != 0;
		buttons.other = (event.state & SDL_BUTTON_MMASK) != 0;

		_touch->Update(position, timestamp, buttons);

		if (_touch->GetGesture() != nullptr && _touch->GetCurrentButtons().Any())
			_touch->GetGesture()->TouchMoved();
	}
}


void Window::ProcessMouseButtonDown(const SDL_MouseButtonEvent& event)
{
	//NSLog(@"ProcessMouseButtonDown button=%d state=%d", event.button, event.state);

	glm::vec2 position = ToVector(event.x, event.y);
	double timestamp = ToTimestamp(event.timestamp);

	MouseButtons buttons;
	if (_touch != nullptr)
		buttons = _touch->GetCurrentButtons();

	switch (event.button)
	{
		case SDL_BUTTON_LEFT: buttons.left = true; break;
		case SDL_BUTTON_RIGHT: buttons.right = true; break;
		case SDL_BUTTON_MIDDLE: buttons.other = true; break;
	}

	if (_touch == nullptr)
		_touch = new Touch(_surface, 1, position, timestamp, buttons);
	else
		_touch->Update(position, timestamp, buttons);

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->TouchBegan(_touch);
}


void Window::ProcessMouseButtonUp(const SDL_MouseButtonEvent& event)
{
	//NSLog(@"ProcessMouseButtonUp button=%d state=%d", event.button, event.state);

	if (_touch != nullptr)
	{
		glm::vec2 position = ToVector(event.x, event.y);
		double timestamp = ToTimestamp(event.timestamp);

		MouseButtons buttons = _touch->GetCurrentButtons();
		switch (event.button)
		{
			case SDL_BUTTON_LEFT: buttons.left = false; break;
			case SDL_BUTTON_RIGHT: buttons.right = false; break;
			case SDL_BUTTON_MIDDLE: buttons.other = false; break;
		}

		_touch->Update(position, timestamp, buttons);

		if (buttons.Any())
		{
			_touch->GetGesture()->TouchMoved();
		}
		else
		{
			if (_touch->GetGesture() != nullptr)
				_touch->GetGesture()->TouchEnded(_touch);

			delete _touch;
			_touch = nullptr;
		}
	}
}


void Window::ProcessMouseWheel(const SDL_MouseWheelEvent& event)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	glm::vec2 position = ToVector(event.x, event.y);

	float k = 1;

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			if (gesture->IsEnabled())
				gesture->ScrollWheel(_surface, position, k * glm::vec2(event.x, event.y));

}


void Window::Update()
{
	double timestamp = current_timestamp();
	double secondsSinceLastUpdate = timestamp - _timestamp;
	_timestamp = timestamp;

	_surface->Update(secondsSinceLastUpdate);

	if (Gesture::_gestures != nullptr)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->Update(_surface, secondsSinceLastUpdate);

	if (_touch != nullptr)
	{
		double oldTimestamp = _touch->GetTimestamp();
		_touch->Update(timestamp);

		if (_touch->GetTimestamp() != oldTimestamp && _touch->GetGesture() != nullptr)
			_touch->GetGesture()->TouchMoved();
	}
}


void Window::Render()
{
	if (_surface->NeedsRender())
	{
		_surface->Render();
		SDL_GL_SwapWindow(_window);
	}
}


glm::vec2 Window::ToVector(int x, int y)
{
	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	return glm::vec2(x, h - y);
}


double Window::ToTimestamp(Uint32 timestamp)
{
	return _timestamp;//timestamp;
}
