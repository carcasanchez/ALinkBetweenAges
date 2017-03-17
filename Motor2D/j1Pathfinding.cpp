#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name = ("pathfinding");
	last_path.clear();
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}


// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);

}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

//Return last path
vector<iPoint> j1PathFinding::ReturnPath()
{
	return last_path;
};

//Return centre of tile



// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
list<PathNode>::iterator PathList::Find(const iPoint& point)
{
	std::list<PathNode>::iterator item = list.begin();
	for (; item != list.cend(); item++)
	{
		if ((*item).pos == point)
			break;

	}

	
	return item;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
list<PathNode>::const_iterator PathList::GetNodeLowestScore() const
{
	int min = 65535;
	
	std::list<PathNode>::const_iterator ret;
	
	for (std::list<PathNode>::const_reverse_iterator item = list.rbegin(); item != list.crend(); item++)
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = item.base();
			ret--;			
		}
	}
	


	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h),pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//diagonals
	/*cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));


	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));


	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));


	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));*/

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceManhattan(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	int ret = -1;

	if (IsWalkable(origin) == false || IsWalkable(destination) == false)
		return ret;

	last_path.clear();

	PathList open_list;
	PathList close_list;

	open_list.list.push_back(PathNode(0, origin.DistanceManhattan(destination), origin, nullptr));



	while (open_list.list.size() > 0)
	{

		std::list<PathNode>::const_iterator Lowest_score_node = open_list.GetNodeLowestScore();
		PathList Adjacents;

		close_list.list.push_back(*Lowest_score_node);
		open_list.list.erase(Lowest_score_node);

		if (close_list.list.back().pos != destination)
		{
			close_list.list.back().FindWalkableAdjacents(Adjacents);

			for (std::list<PathNode>::iterator Adjacents_item = Adjacents.list.begin(); Adjacents_item != Adjacents.list.end(); Adjacents_item++)
			{
				if (close_list.Find((*Adjacents_item).pos) != close_list.list.end())
					continue;


				std::list<PathNode>::iterator temp = open_list.Find((*Adjacents_item).pos);
				if (temp != open_list.list.end())
				{
					(*Adjacents_item).CalculateF(destination);
					if ((*Adjacents_item).g < (*temp).g)
						(*temp).parent = (*Adjacents_item).parent;
				}
				else
				{
					(*Adjacents_item).CalculateF(destination);
					open_list.list.push_back((*Adjacents_item));
				}

			}

		}
		else
		{
			const PathNode* tem = &close_list.list.back();
			while (tem->parent != nullptr)
			{
				last_path.push_back(tem->pos);
				tem = tem->parent;
				ret++;
			}
			last_path.push_back(tem->pos);
			ret++;

			vector<iPoint>::iterator the_begin = last_path.begin();
			vector<iPoint>::reverse_iterator the_end = last_path.rbegin();
			for (; (*the_begin) != (*the_end) && (*the_begin) != (*(the_end + 1)) ; the_begin++, the_end++)
				std::iter_swap(the_begin, the_end);

			break;
		}
	}

	return ret;
}
