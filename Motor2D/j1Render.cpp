#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1GameLayer.h"
#include "Player.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "j1CollisionManager.h"
#include "j1Map.h"
#include "j1Textures.h"
#include "Bezier.h"

#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name = ("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;

		renderZone.x = renderZone.y = 0;
		renderZone.w = App->win->screen_surface->w / App->win->GetScale();
		renderZone.h = App->win->screen_surface->h / App->win->GetScale();

	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	SetViewPort({ 0, 0, camera.w, camera.h });

	//initialize sprite map
	for (int i = 0; i < int(FRONT); i++)
	{
		std::multimap<int, Sprite*> map;
		std::pair<spriteLayer, std::multimap<int, Sprite*>> pair = { spriteLayer(i), map };
	}

	fade_bezier = new CBeizier();

	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{	
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::PostUpdate()
{
/*	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
	{
		freeCamera =! freeCamera;
	}

	if (freeCamera)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			camera.y++;
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			camera.y--;
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			camera.x--;
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			camera.x++;
		}
	}*/


	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);


	App->map->Draw();

	std::map<spriteLayer, std::multimap<int, Sprite*>>::iterator layer;
	for (layer = spriteMap.begin(); layer != spriteMap.end(); layer++)
	{
		// blit sprites
		std::multimap<int, Sprite* > ::iterator it;
		for (it = layer->second.begin(); it != layer->second.end(); it++)
		{
			Sprite* sprite = it->second;
			CompleteBlit(sprite->texture, sprite->position_map.x - sprite->pivot.x, sprite->position_map.y - sprite->pivot.y, sprite->section_texture, sprite->tint, sprite->angle, sprite->pivot.x, sprite->pivot.y, sprite->flip);
		}

		layer->second.clear();
	}

	App->map->DrawOver();
	PrintUI();
	App->collisions->DrawDebug();

	// cover culling zone
	if (App->win->isFullScreen())
	{
		SDL_Rect rect = { 1,1,0,0 };
		rect.w = App->win->screen_surface->w - renderZone.w;
		rect.h = App->win->screen_surface->h;

		CompleteBlit(App->tex->GetDefault("black"), renderZone.x + renderZone.w, 0, rect);
	}

	//hide culling

	if (in_fade)
		DrawQuad({(int)(-camera.x * App->gui->scale_factor) , (int)(-camera.y *  App->gui->scale_factor), camera.w, camera.h}, 0, 0, 0, fade_alpha);
	


	SDL_RenderPresent(renderer);

	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	RELEASE(fade_bezier);
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

iPoint j1Render::WorldToScreen(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x + camera.x / scale);
	ret.y = (y - camera.x / scale);

	return ret;
}

void j1Render::CenterCamera(iPoint pos)
{
	int scale = App->win->GetScale();
	uint w, h;

	camera.x = -(pos.x * scale);
	camera.y = -(pos.y * scale);
	camera.x += SDL_ceil((camera.w)*0.5);
	camera.y += SDL_ceil((camera.h)*0.5);

	renderZone.x = pos.x;
	renderZone.y = pos.y;
	renderZone.x -= renderZone.w*0.5;
	renderZone.y -= renderZone.h*0.5;
}

bool j1Render::Draw(Sprite* sprite)
{
	bool ret = true;

	if (ret = (sprite != NULL))
	{
		if (ret = (sprite->texture != NULL))
		{
			std::pair<int, Sprite*> pair = { sprite->y, sprite };

			try // check for out-of-range throw
			{
				spriteMap.at(sprite->layer).insert(pair);
			}
			catch (const std::out_of_range& oor)
			{
				std::multimap<int, Sprite*> map;
				map.insert(pair);
				std::pair<spriteLayer, std::multimap<int, Sprite*>> layer = { sprite->layer, map };
				spriteMap.insert(layer);
			}
		}
		else
		{
			LOG("Render: Error, invalid texture to blit.");
		}
	}
	else
	{
		LOG("Render: Error, invalid sprite to blit.");
	}

	return ret;
}

//Sprite
bool j1Render::DrawSprite(Sprite* sprite, float speed, double angle, int pivot_x, int pivot_y)
{
	bool ret = true;

	Blit(sprite->texture, sprite->position_map.x - sprite->pivot.x, sprite->position_map.y - sprite->pivot.y, &(sprite->section_texture));

	return ret;

}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, int alpha, float speed, double angle, int pivot_x, int pivot_y, SDL_RendererFlip flip) const
{

	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	Uint8 currentAlpha;
	SDL_GetTextureAlphaMod(texture, &currentAlpha);
	if (alpha != currentAlpha)
	{
		SDL_SetTextureAlphaMod(texture, (Uint8)alpha);
	}


	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		//LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::CompleteBlit(SDL_Texture* texture, int x, int y, const SDL_Rect section, SDL_Color tint, double angle, int pivot_x, int pivot_y, SDL_RendererFlip flip) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	if (!(ret = (texture != NULL)))
	{
		return ret;
	}

	// check if inside camera's view
	if (!(ret = InsideCameraZone({ x, y, section.w, section.h })))
	{
		return ret;
	}

	// get correct measures to render
	SDL_Rect rect;
	rect.x = (int)(camera.x) + x * scale;
	rect.y = (int)(camera.y) + y * scale;


	if (section.h > 0 && section.w > 0)
	{
		if (ret = (section.w >= 0 && section.h >= 0))
		{
			rect.w = section.w;
			rect.h = section.h;
		}
		else
		{
			return ret;
		}
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	// set pivot offset
	SDL_Point* p = NULL;
	SDL_Point pivot;
	if (pivot_x != 0 && pivot_y != 0)
	{
		pivot.x = camera.x + pivot_x * scale;
		pivot.y = camera.y + pivot_y * scale;
		p = &pivot;
	}

	
	// apply tint
	if (!(ret = (SDL_SetTextureColorMod(texture, tint.r, tint.g, tint.b) == 0)))
	{
		LOG("Invalid Texture color modulation");
		return ret;
	}

	if (tint.a != 0)
	{
		if (!(ret = (SDL_SetTextureAlphaMod(texture, tint.a) == 0)))
		{
			LOG("Invalid Texture alpha modulation");
			return ret;
		}
	}

	if (section.h > 0 && section.w > 0)
	{
		if (!(ret = (SDL_RenderCopyEx(renderer, texture, &section, &rect, angle, p, flip) == 0)))
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		}
	}
	else
	{
		if (!(ret = (SDL_RenderCopyEx(renderer, texture, NULL, &rect, angle, p, flip) == 0)))
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		}
	}

	//App->render->DrawQuad(renderZone, 255, 0, 0, 20);

	return ret;
}

bool j1Render::InsideCameraZone(SDL_Rect rect)const
{

	SDL_Rect res;
	bool a = SDL_IntersectRect(&rect, &renderZone, &res);
			
	return a;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

void j1Render::CameraFollow(iPoint pos)
{

	int scale = App->win->GetScale();
	uint w, h;


	if (!freeCamera)
	{
		
		camera.x = -(pos.x * scale);
		camera.y = -(pos.y * scale);
		camera.x += SDL_ceil((camera.w)*0.5);
		camera.y += SDL_ceil((camera.h)*0.5);
	
		renderZone.x = pos.x;
		renderZone.y = pos.y;
		renderZone.x -= renderZone.w*0.5;
		renderZone.y -= renderZone.h*0.5;

		if (cameraLocked)
			return;

		if (renderZone.x < 0)
		{
			camera.x = 0;
			renderZone.x = 0;
		}
		else if (renderZone.x + renderZone.w >= (App->map->data->width)*App->map->data->tile_width)
		{
			renderZone.x = (App->map->data->width)*App->map->data->tile_width - renderZone.w;
			camera.x = -renderZone.x* scale;
		}

		if (renderZone.y < 0)
		{
			camera.y = 0;
			renderZone.y = 0;
		}
		else if (renderZone.y + renderZone.h >= (App->map->data->height)*App->map->data->tile_height)
		{
			renderZone.y = (App->map->data->height)*App->map->data->tile_height - renderZone.h;
			camera.y = (-renderZone.y)* scale;
		}
	}
	
}

void j1Render::DebugCamera()
{
	DrawQuad(renderZone, 0, 255, 255, 80);
}

bool j1Render::EraseUiElement(UI_element* erased_element)
{
	bool ret = false;

	for (std::list<UI_element*>::iterator it = ui_elements.begin(); it != ui_elements.end(); it++)
	{
		if ((*it) == erased_element)
		{
			ui_elements.erase(it);
			ret = true;
			break;
		}
	}
	return ret;
}

void j1Render::IntoFade()
{
	in_fade = true;
}

void j1Render::StopFade()
{
	in_fade = false;
}

void j1Render::SetAlpha(int value)
{
	fade_alpha = value;
}

bool j1Render::PrintUI()
{
	for (std::list<UI_element*>::iterator it = ui_elements.begin(); it != ui_elements.end(); it++)
	{
		if ((*it)->active)
			(*it)->Update_Draw();
	}

	return true;
}

Sprite::Sprite() : texture(NULL), position_map({ 0, 0 }), section_texture({ 0, 0, 0, 0 }), tint({ 255, 255, 255, 0 }),
flip(SDL_FLIP_NONE), layer(SCENE), angle(0)
{}

Sprite::Sprite(SDL_Texture* texture, iPoint position, spriteLayer layer, SDL_Rect section,
	iPoint pivot, SDL_Color tint, double angle, SDL_RendererFlip flip) :
	texture(texture), 
	position_map(position), pivot(pivot), section_texture(section), tint(tint),
	flip(flip), layer(layer), angle(angle)
{}

Sprite::~Sprite()
{
	App->tex->UnLoad(texture);
}

void Sprite::updateSprite(SDL_Texture* tex, iPoint& p, iPoint& piv, SDL_Rect& section, SDL_RendererFlip flip)
{
	texture = tex;
	position_map = p;
	section_texture = section;
	pivot = piv;
	y = p.y;
	this->flip = flip;

}

void Sprite::updateSprite(iPoint& p, iPoint& piv, SDL_Rect& section, SDL_RendererFlip flip)
{
	position_map = p;
	section_texture = section;
	pivot = piv;
	y = p.y;
	this->flip = flip;
}

void Sprite::setAlpha(int alpha)
{
	if (alpha >= 0 && alpha <= 255)
	{
		tint.a = alpha;
	}
}