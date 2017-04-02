#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "SDL\include\SDL.h"
#include "p2Point.h"
#include "j1Module.h"
#include "Quadtree.h"

#define NON_WALKABLE_TILE 50

// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		string name;
		int value;
	};

	~Properties()
	{
		
		for (std::list<Property*>::iterator item = list.begin(); item != list.cend(); item++)
			RELEASE(*item);

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	list<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	string	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	string				name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

struct MapCluster
{
	MapCluster() {};
	SDL_Rect rect;
	std::vector<iPoint> tiles;
};


enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	list<TileSet*>	tilesets;
	list<MapLayer*>	layers;
};


// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	bool Start();

	bool Update(float dt);

	// Called each loop iteration
	void Draw();
	void DrawOver();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	iPoint WorldToMapMouse(int x, int y) const;
	iPoint GetTileCenter(iPoint);

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer, uchar** buffer2) const;

	bool LoadSmallDataMap(const char* path);
	void UnloadSmallDataMap();


	//Debug pathfinding
	void DebugPath(vector<iPoint>);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);

	TileSet* GetTilesetFromTileId(int id) const;

	void UnLoadData();

public:

	MapData* data;

private:

	MapData smallData;
	MapData bigData;

	pugi::xml_document*	map_file;
	pugi::xml_document	small_map_file;
	pugi::xml_document	normal_map_file;

	std::vector<MapCluster> clusters;

	string				folder;
	bool				map_loaded;
	bool				debug_collisions = false;
	bool				debug_path = false;
	bool				debug_enemy_collisions = false;


	SDL_Texture* debug_tex;
};

#endif // __j1MAP_H__