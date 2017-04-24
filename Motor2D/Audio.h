#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "j1Module.h"
#include <list>
#include <map>

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

struct Song
{
	std::string name;
	std::string path;
	float fadeTime;
};

struct Sound
{
	std::string name;
	std::string path;
};


class Audio : public j1Module
{
public:

	Audio();
	virtual ~Audio();

	bool Awake(pugi::xml_node&);
	bool Update(float);
	bool CleanUp();
	
	void LoadSound(std::string, std::string);
	void LoadSong(std::string, std::string, float);
	
	void ReleaseSceneAudio();

	void PlayMusic(std::string);
	void PlayFx(std::string);

public:

	float volume = 50.0f;

private:

	// Play a music file
	bool PlayMusic(const char*, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	// Load a WAV in memory
	unsigned int LoadFx(const char*);

private:

	std::string soundsFolder;
	std::string songsFolder;

	std::map<string, Sound> sounds;
	std::map<string, Song> songs;

	_Mix_Music*			music;
	std::list<Mix_Chunk*>	fx;
};

#endif // __AUDIO_H__