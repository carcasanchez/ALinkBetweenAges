#ifndef _UI_STRING_H_
#define _UI_STRING_H_

#include "UI_element.h"
#include "j1Timer.h"

enum TEXTSTATE
{
	FINISHED_TEXT,
	MID_TEXT,
	
};

struct _TTF_Font;
struct SDL_Texture;

class UI_String : public UI_element
{
public:
	string			text;
	int				text_size;
	_TTF_Font*		text_font = nullptr;
	SDL_Texture*	text_texture = nullptr;

	TEXTSTATE		dialog_state = FINISHED_TEXT;
	int				blit_time = 0;

public:

	UI_String(UI_TYPE, j1Module*);
	UI_String(UI_TYPE type, SDL_Rect detection_box, char* new_text, bool active = true, SCROLL_TYPE draggable = FREE_SCROLL);
	UI_String(const UI_String* other);

	~UI_String();

	
	bool Set_String(char* new_text);				//Changes the string
	void Load_text_texture();						//Loads the text texture
	
	bool Update();
	bool Update_Draw();
	bool Handle_input();
		
	void SetBlitTimeMS(int);
	void ForcedFinish();

private:

	void LookEnter();

	string	blit_text;
	j1Timer	char_blit_time;
	void	BlitDialog();
	

};

#endif // ! _UI_STRING_H_

