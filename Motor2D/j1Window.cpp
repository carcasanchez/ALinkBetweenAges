#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"

#include "SDL/include/SDL.h"


j1Window::j1Window() : j1Module()
{
	window = NULL;
	screen_surface = NULL;
	name = ("window");
}

// Destructor
j1Window::~j1Window()
{
}

// Called before render is available
bool j1Window::Awake(pugi::xml_node& config)
{
	LOG("Init SDL window & surface");
	bool ret = true;

	SDL_DisplayMode mode;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else if (SDL_GetNumDisplayModes(0) < 1)
	{
		LOG("No available displays found.");
		ret = false;
	}
	else if (SDL_GetDisplayMode(0, 0, &mode) != 0)
	{
		LOG("Could not get display information");
		ret = false;
	}
	else
	{
		width = config.child("resolution").attribute("width").as_int(256);
		height = config.child("resolution").attribute("height").as_int(224);
		scale = config.child("resolution").attribute("scale").as_int(1);
		int h_margin = config.child("margin").attribute("horizontal").as_int(0);
		int v_margin = config.child("margin").attribute("vertical").as_int(0);

		// check display has minimum dimensions
		if (mode.w < width || mode.h < height)
		{
			LOG("Display too small. Setting scale to 1 (minimum value)");
			scale = 1;
		}
		else
		{
			while (width * (scale + 1) + (2 * h_margin) < mode.w && height * (scale + 1) + (2 * v_margin) < mode.h)
			{
				scale++;
			}

			//scale dimensions
			width *= scale;
			height *= scale;
		}

		//Create window
		Uint32 flags = SDL_WINDOW_SHOWN;

		if(fullScreen = config.child("fullscreen").attribute("value").as_bool(false))
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(config.child("borderless").attribute("value").as_bool(false))
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(config.child("resizable").attribute("value").as_bool(false) == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(config.child("fullscreen_window").attribute("value").as_bool(false) == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetTitle(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}

		uint a, b;
		GetWindowSize(a, b);
		renderZone.x = a / scale;
		renderZone.y = b / scale;
	}

	return ret;
}

// Called before quitting
bool j1Window::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

// Set new window title
void j1Window::SetTitle(const char* new_title)
{
	//title.create(new_title);
	SDL_SetWindowTitle(window, new_title);
}

void j1Window::GetWindowSize(uint& width, uint& height) const
{
	width = this->width;
	height = this->height;
}

uint j1Window::GetScale() const
{
	return scale;
}

iPoint j1Window::GetRenderZone()
{
	return renderZone;
}

bool j1Window::isFullScreen() const
{
	return fullScreen;
}