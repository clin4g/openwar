#ifndef Window_H
#define Window_H

#import <SDL2/SDL.h>
#import "Touch.h"

class Surface;


class Window
{
	static bool _done;
	static std::map<Uint32, Window*> _windows;

	Surface* _surface;
	SDL_Window* _window;
	SDL_GLContext _glcontext;
	Touch* _touch;
	double _timestamp;

public:
	Window();
	~Window();

	void SetSurface(Surface* surface) { _surface = surface; }

	static bool IsDone();
	static void ProcessEvents();

private:
	static void ProcessEvent(const SDL_Event& event);

	void ProcessWindow(const SDL_WindowEvent& event);
	void ProcessKeyDown(const SDL_KeyboardEvent& event);
	void ProcessKeyUp(const SDL_KeyboardEvent& event);
	void ProcessMouseMotion(const SDL_MouseMotionEvent& event);
	void ProcessMouseButtonDown(const SDL_MouseButtonEvent& event);
	void ProcessMouseButtonUp(const SDL_MouseButtonEvent& event);
	void ProcessMouseWheel(const SDL_MouseWheelEvent& event);

	void Update();
	void Render();

	static Window* GetWindow(Uint32 windowID);

	glm::vec2 ToVector(int x, int y);
	double ToTimestamp(Uint32 timestamp);
};


#endif
