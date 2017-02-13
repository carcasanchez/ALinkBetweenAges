#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "p2Point.h"
#include "SDL\include\SDL_rect.h"
#include "Module_entites_manager.h"

class entity
{
public:

	entity();
	entity(p2Point<int> pos);

	virtual bool Start() { return true; };
	virtual bool PreU() { return true; };
	virtual bool U(float dt) { return true; };
	virtual bool UTicks() { return true; };
	virtual bool PostU() { return true; };

	void is_Selected();
	void not_Selected();

	void Set_texture(SDL_Rect);

	SDL_Rect					player_text;
	iPoint						position;
	int							last_point = 0;
	int							path_length = 0;

	int							speed;

	bool						selected = false;
	bool						selectionable;

};

#endif // !_ENTITY_H_
