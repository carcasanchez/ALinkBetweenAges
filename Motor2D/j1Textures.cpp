#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1FileSystem.h"
#include "j1Textures.h"

#include "SDL_image/include/SDL_image.h"
#pragma comment( lib, "SDL_image/libx86/SDL2_image.lib" )

j1Textures::j1Textures() : j1Module()
{
	name = ("textures");
}

// Destructor
j1Textures::~j1Textures()
{}

// Called before render is available
bool j1Textures::Awake(pugi::xml_node& config)
{
	LOG("Init Image library");
	bool ret = true;
	// load support for the PNG image format
	int flags = IMG_INIT_PNG;
	int init = IMG_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		ret = false;
	}
	else
	{
		std::string folder = config.attribute("folder").as_string("/");
		for (pugi::xml_node tex = config.child("texture"); !tex.empty(); tex = tex.next_sibling())
		{
			std::string path = folder + tex.attribute("file").as_string();
			std::string name = tex.attribute("name").as_string();
			pair<std::string, std::string> pair = { name, path };
			defaultData.push_back(pair);
		}
	}

	return ret;
}

// Called before the first frame
bool j1Textures::Start()
{
	bool ret = true;

	list<pair<std::string, std::string>>::iterator item = defaultData.begin();

	while (!defaultData.empty())
	{
		pair<std::string, SDL_Texture*> pair = { item->first, Load(item->second.c_str()) };

		if (pair.second != NULL)
		{
			defaultTextures.insert(pair);
		}
		else
		{
			LOG("Couldn't load default texture %s @%s", item->first.c_str(), item->second.c_str());
		}

		defaultData.erase(item);
	}

	return ret;
}

// Called before quitting
bool j1Textures::CleanUp()
{
	LOG("Freeing textures and Image library");

	for (list<SDL_Texture*>::iterator item = textures.begin(); item != textures.end(); item++)
	{
			SDL_DestroyTexture((*item));
	}

	textures.clear();
	IMG_Quit();
	return true;
}

// Load new texture from file path
SDL_Texture* const j1Textures::Load(const char* path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load_RW(App->fs->Load(path), 1);

	if(surface == NULL)
	{
		LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

// Unload texture
bool j1Textures::UnLoad(SDL_Texture* texture)
{
	for (list<SDL_Texture*>::iterator item = textures.begin(); item != textures.end(); item++)
	{
		if (texture == *item)
		{
			SDL_DestroyTexture(*item);
			textures.erase(item);
			return true;
		}
	}

	return false;
}

// Translate a surface into a texture
SDL_Texture* const j1Textures::LoadSurface(SDL_Surface* surface)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(App->render->renderer, surface);

	if(texture == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		textures.push_back(texture);
	}

	return texture;
}

// Retrieve size of a texture
void j1Textures::GetSize(const SDL_Texture* texture, uint& width, uint& height) const
{
	SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, (int*) &width, (int*) &height);
}

SDL_Texture* j1Textures::GetDefault(std::string name) const
{
	SDL_Texture* ret;

	try // check for out-of-range throw
	{
		ret = defaultTextures.at(name);
	}
	catch (const std::out_of_range& oor)
	{
		ret = NULL;
	}

	return ret;
}