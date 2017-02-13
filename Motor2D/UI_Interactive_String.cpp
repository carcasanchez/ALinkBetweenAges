#include "UI_Interactive_String.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Fonts.h"

UI_Interactive_String::UI_Interactive_String(UI_TYPE type, SDL_Rect detection_box, const char* new_text) : UI_element(type, detection_box), text(new_text) {}

UI_Interactive_String::UI_Interactive_String(const UI_Interactive_String* other) : UI_element(other->element_type, other->Interactive_box), text(other->text) {}

const char* UI_Interactive_String::Get_String() const
{
	return text;
}

bool UI_Interactive_String::Set_String(const char* new_text)
{
	text = new_text;
	return (text != nullptr) ? true : false;
}

bool UI_Interactive_String::Update()
{
	int x, y;
	App->input->GetMousePosition(x, y);

	if (Mouse_is_in({ x, y }))
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			state = CLICK_ELEMENT;
		else state = OVER_ELEMENT;
	}
	else state = NOTHING;

	return true;
}

bool UI_Interactive_String::Update_Draw()
{
	//App->render->Blit(App->font->Print(text), position.x - App->render->camera.x, position.y - App->render->camera.y);

	if (Parent)
		App->render->Blit(App->font->Print(text), Parent->Interactive_box.x + (Interactive_box.x - App->render->camera.x), Parent->Interactive_box.y + (Interactive_box.y - App->render->camera.y));

	else App->render->Blit(App->font->Print(text), Interactive_box.x - App->render->camera.x, Interactive_box.y - App->render->camera.y);

	Child_Update_Draw();

	return true;
}