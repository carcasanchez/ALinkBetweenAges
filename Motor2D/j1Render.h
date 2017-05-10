#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "p2Point.h"
#include <map>

class UI_element;

enum spriteLayer
{
	BACKGROUND = 0,
	SCENE,
	MENU,
	FADER,
	UI,
	FRONT
};

struct Sprite
{
	Sprite();
	Sprite(SDL_Texture* texture, iPoint position = { 0, 0 },
		spriteLayer layer = SCENE, SDL_Rect section = { 0, 0, 0, 0 },
		iPoint pivot = { 0, 0 }, SDL_Color tint = { 255, 255, 255, 0 },
		double angle = 0, SDL_RendererFlip flip = SDL_FLIP_NONE);
	~Sprite();

	void updateSprite(SDL_Texture* tex, iPoint& p, iPoint& piv, SDL_Rect& section, SDL_RendererFlip flip);
	void updateSprite(iPoint& p, iPoint& piv, SDL_Rect& section, SDL_RendererFlip flip);

	void setAlpha(int alpha);

	SDL_Texture*		texture;
	iPoint				position_map;
	iPoint				pivot;
	SDL_Rect			section_texture;
	SDL_Color			tint;
	SDL_RendererFlip	flip;

	int				y;
	double			angle;
	spriteLayer		layer;

};

enum simpleGeoFigureTYPE;
struct SimpleGeoFigure;

class j1Render : public j1Module
{
public:

	j1Render();

	// Destructor
	virtual ~j1Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Utils
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	iPoint ScreenToWorld(int x, int y) const;
	iPoint WorldToScreen(int x, int y) const;
	void CenterCamera(iPoint pos);
	void SetBackgroundColor(SDL_Color color);
	bool InsideCameraZone(SDL_Rect rect) const;
	bool freeCamera = false;

	// Sprite handling
	bool Draw(Sprite* sprite);
	bool DrawSprite(Sprite* sprite, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX);

	// Draw & Blit
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, int alpha = 255, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool CompleteBlit(SDL_Texture* texture, int x, int y, const SDL_Rect section, SDL_Color tint = { 255,255,255,0 }, double angle = 0, int pivot_x = 0, int pivot_y = 0, SDL_RendererFlip flip = SDL_FLIP_NONE) const;
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	void CameraFollow(iPoint pos);
	void DebugCamera();

	//UI
	bool EraseUiElement(UI_element*);

private:

	bool PrintUI();
	

public:

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		renderZone;
	SDL_Rect		viewport;
	SDL_Color		background;

	bool cameraLocked = false;
	


	std::map<spriteLayer, std::multimap<int, Sprite*>> spriteMap;

	std::multimap<int, SimpleGeoFigure*> debugFigures;

	std::list<UI_element*> ui_elements;
};

#endif // __j1RENDER_H__