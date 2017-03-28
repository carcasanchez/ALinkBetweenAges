#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), player_map(NULL), enemy_map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name = ("pathfinding");
	last_path.clear();
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(player_map);
	RELEASE_ARRAY(enemy_map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(player_map);
	RELEASE_ARRAY(enemy_map);
	RELEASE_ARRAY(node_map);

	return true;
}


// Sets up the walkability map
void j1PathFinding::SetPlayerMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(player_map);
	player_map = new uchar[width*height];
	memcpy(player_map, data, width*height);

}

void j1PathFinding::SetEnemyMap(uint width, uint height, uchar * data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(enemy_map);
	enemy_map = new uchar[width*height];
	memcpy(enemy_map, data, width*height);

	RELEASE_ARRAY(node_map);
	node_map = new PathNode[width*height];
	
}


// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsPlayerWalkable(const iPoint& pos) const
{
	uchar t = GetTileForPlayer(pos);
	return t != INVALID_WALK_CODE && t != 0;
}

bool j1PathFinding::IsEnemyWalkable(const iPoint& pos) const
{
	uchar t = GetTileForEnemy(pos);
	return t != INVALID_WALK_CODE && t != 0;

}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileForPlayer(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return player_map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

uchar j1PathFinding::GetTileForEnemy(const iPoint & pos) const
{
	if(CheckBoundaries(pos))
		return enemy_map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}


PathNode * j1PathFinding::GetPathNode(int x, int y)
{
	return &node_map[(y*width) + x];
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
/*list<PathNode>::iterator PathList::Find(const iPoint& point)
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
{}*/

PathNode::PathNode(int g, int h, const iPoint& pos, PathNode* parent) : g(g), h(h),pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}


// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill, const iPoint& destination)
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill.list.push(node);
	}
	

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill.list.push(node);
	}

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill.list.push(node);
	}

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}
		list_to_fill.list.push(node);
	}

	//diagonals
	/*cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}

		list_to_fill.list.push(node);
	}


	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}

		list_to_fill.list.push(node);
	}


	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}

		list_to_fill.list.push(node);
	}


	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsEnemyWalkable(cell))
	{
		PathNode* node = App->pathfinding->GetPathNode(cell.x, cell.y);
		if (node->pos != cell) {
			node->parent = this;
			node->pos = cell;
		}

		list_to_fill.list.push(node);
	}*/

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

	if (IsEnemyWalkable(origin) == false || IsEnemyWalkable(destination) == false)
		return ret;

	last_path.clear();

	PathList open_list;
	PathNode* first_node = new PathNode(0, origin.DistanceManhattan(destination), origin, nullptr);
	open_list.list.push(first_node);
	std::fill(node_map, node_map + width*height, PathNode(-1, -1, iPoint(-1, -1), nullptr));

	PathNode* current = nullptr;

	while (open_list.list.size() > 0)
	{

		current = open_list.list.top();
		current->onClose = true;
		open_list.list.pop();

		if (current->pos == destination)
		{
			last_path.clear();


			for (; current->parent != nullptr; current = current->parent)
			{
				last_path.push_back(current->pos);

			}
			last_path.push_back(current->pos);
			ret = true;
			break;
		}
		else
		{

			PathList neighbours;
			current->FindWalkableAdjacents(neighbours, destination);

			while(neighbours.list.empty() == false) {
				
				PathNode* temp = neighbours.list.top();
				neighbours.list.pop();
				
				if (temp->onClose == true)
				{
					continue;
				}
				else if (temp->onOpen == true)
				{
					int last_g_value = temp->g;
					temp->CalculateF(destination);
					if (last_g_value < temp->g)
					{
						temp->parent = GetPathNode(current->pos.x, current->pos.y);
					}
					else
					{
						temp->g = last_g_value;
					}
				}
				else
				{
					temp->CalculateF(destination);

					temp->onOpen = true;
					open_list.list.push(temp);

				}
			}

		}
		
	}

	return ret;
}
