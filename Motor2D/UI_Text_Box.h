#ifndef _UI_TEXT_BOX_H_
#define _UI_TEXT_BOX_H_

#include "UI_element.h"
#include "UI_Image.h"
#include "UI_String.h"

class UI_Text_Box : public UI_element
{
public:

	UI_String text;
	const UI_Image* background;
	int cursor_pos, height;
	int cursor_virtual_pos;

public:

	UI_Text_Box(UI_TYPE, j1Module*);
	UI_Text_Box(UI_TYPE type, SDL_Rect detection_box, char* new_text, const UI_Image* start_image, bool active = true, SCROLL_TYPE draggable = FREE_SCROLL);
	UI_Text_Box(const UI_Text_Box* other);

	void text_box_state();

	bool Update();
	bool Handle_input();
	void Text_management();
	bool Update_Draw();

	const char* get_string_pos(int cursor_pos);
	void Insert_Char(int cursor_pos, const char* new_char);
	void Delete_Char(int cursor_pos);

	void Set_Background(UI_Image*);
	
};



#endif // !_UI_TEXT_BOX_H_

