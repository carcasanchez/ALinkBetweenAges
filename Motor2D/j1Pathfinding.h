#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "p2Point.h"
#include <queue>

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

struct PathNode;

class j1PathFinding : public j1Module
{
public:

	j1PathFinding();

	// Destructor
	~j1PathFinding();

	// Called before quitting
	bool CleanUp();

	// Sets up the walkability map
	void SetPlayerMap(uint width, uint height, uchar* data);
	void SetEnemyMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B
	int CreatePath(const iPoint& origin, const iPoint& destination);


	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsPlayerWalkable(const iPoint& pos) const;
	bool IsEnemyWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileForPlayer(const iPoint& pos) const;
	uchar GetTileForEnemy(const iPoint& pos) const;

	PathNode* GetPathNode(int x, int y);


	//Return path
	vector<iPoint> ReturnPath();

	

	

private:


	PathNode* node_map = nullptr;
	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	uchar* player_map;
	uchar* enemy_map;
	// we store the created path here
	vector<iPoint> last_path;
};

// forward declaration
struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode() {};
	PathNode(int g, int h, const iPoint& pos, PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill, const iPoint& destination);
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	// -----------
	int g;
	int h;
	iPoint pos;
	bool onClose = false;
	bool onOpen = false;
	PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------

struct compare
{
	bool operator()(const PathNode* l, const PathNode* r)
	{
		return l->Score() >= r->Score();
	}
};

struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	//list<PathNode>::iterator Find(const iPoint& point);

	// Returns the Pathnode with lowest score in this list or NULL if empty
	//list<PathNode>::const_iterator GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	std::priority_queue<PathNode*, std::vector<PathNode*>, compare > list;
	//std::list<PathNode> list;
};



#endif // __j1PATHFINDING_H__
