#ifndef __j1TEXTURES_H__
#define __j1TEXTURES_H__

#include "j1Module.h"
#include "p2Defs.h"
#include <list>
#include <map>
#include <string>

struct SDL_Texture;
struct SDL_Surface;

class j1Textures : public j1Module
{
public:

	j1Textures();

	// Destructor
	virtual ~j1Textures();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	// Load Texture
	SDL_Texture* const	Load(const char* path);
	bool				UnLoad(SDL_Texture* texture);
	SDL_Texture* const	LoadSurface(SDL_Surface* surface);
	void				GetSize(const SDL_Texture* texture, uint& width, uint& height) const;
	bool				UnloadVideo(SDL_Texture* texture);
	SDL_Texture* const  LoadSurfaceVideo(SDL_Surface* surface);


	// Get Default Tecture
	SDL_Texture* GetDefault(std::string) const;

public:

	list<SDL_Texture*>	textures;
	std::vector<SDL_Texture*> texturesVideo;

private:

	map<std::string,SDL_Texture*>	defaultTextures;
	list<pair<std::string, std::string>> defaultData;
};


#endif // __j1TEXTURES_H__