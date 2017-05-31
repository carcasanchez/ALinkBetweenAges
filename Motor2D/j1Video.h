#ifndef __j1VIDEO_H__
#define __j1VIDEO_H__

#include "j1Module.h"
#include "SDL\include\SDL.h"

#include <dshow.h>
#include <Vfw.h>

#pragma comment( lib, "vfw32.lib" )	
//-------




class j1Video : public j1Module
{
public:

	j1Video();

	// Destructor
	virtual ~j1Video();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	void Initialize(char* file_path);
	void OpenAvi(LPCSTR path);
	bool GrabAVIFrame();

	void CloseAVI();										// Properly Closes The Avi File

public:




	AVISTREAMINFO		psi;										// Pointer To A Structure Containing Stream Info
	PAVISTREAM			pavi;										// Handle To An Open Stream
	PGETFRAME			pgf;										// Pointer To A GetFrame Object


	int					frame = 0;
	long				lastframe;									// Last Frame Of The Stream
	int					width;										// Video Width
	int					height;										// Video Height
	char				*pdata;										// Pointer To Texture Data


	const char*			wString;									//Path to our video


};

#endif // __j1AUDIO_H__#pragma once
