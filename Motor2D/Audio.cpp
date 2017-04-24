#include "Audio.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1FileSystem.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

Audio::Audio() : j1Module(), music(nullptr)
{
	name.assign("audio");
	sounds.clear();
	songs.clear();
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = active = false;
	}

	if (ret)
	{
		int flags = MIX_INIT_OGG;
		int init = Mix_Init(flags);

		if ((init & flags) != flags)
		{
			LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
			ret = active = false;
		}
	}

	if (ret)
	{
		//Initialize SDL_mixer
		if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			ret = active = false;
		}
	}

	if (ret)
	{
		//Save folder paths
		soundsFolder.assign(config.child("sounds").attribute("folder").as_string());
		songsFolder.assign(config.child("songs").attribute("folder").as_string());
	}

	return ret;
}

//update
bool Audio::Update(float dt)
{
	Mix_VolumeMusic(volume);
	return true;
}


// Called before quitting
bool Audio::CleanUp()
{
	bool ret = true;

	if (active)
	{
		ReleaseSceneAudio();
		Mix_CloseAudio();
		Mix_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}

	return ret;
}

void Audio::LoadSound(std::string name, std::string path)
{
	
}

void Audio::LoadSong(std::string name, std::string path, float fadeTime = DEFAULT_MUSIC_FADE_TIME)
{

}

void Audio::ReleaseSceneAudio()
{
	sounds.clear();
	songs.clear();

	if (music != nullptr)
		Mix_FreeMusic(music);

	list<Mix_Chunk*>::iterator item;
	for (item = fx.begin(); item != fx.end(); ++item)
		Mix_FreeChunk(*item);

	fx.clear();
}

void Audio::PlayMusic(std::string)
{

}

void Audio::PlayFx(std::string)
{

}

















// Play a music file
bool Audio::PlayMusic(const char* path, float fadeTime)
{
	bool ret = true;
	active = true;

	//app->audio->PlayMusic("audio/music/introedit.ogg");
	
	if(!active)
		return false;
	
	if(music != nullptr)
	{
		if(fadeTime > 0.0f)
		{
			Mix_FadeOutMusic(int(fadeTime * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS_RW(App->fs->Load(path), 1);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fadeTime > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fadeTime * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;
	active = true;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV_RW(App->fs->Load(path), 1);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
//Maybe i cause errors here, i can create a [] operator for here
bool Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;
	active = true;

	if(!active)
		return false;

	if(id > 0 && id <= fx.size())
	{
		Mix_Chunk* chunk = NULL;
		list<Mix_Chunk*>::iterator item = fx.begin();

		for (int i = 0; i < id; ++i, ++item)
		{
			chunk = *item;
		}

		Mix_PlayChannel(-1, chunk, repeat);
	}

	return ret;
}