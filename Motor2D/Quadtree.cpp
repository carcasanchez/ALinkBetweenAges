#include "Quadtree.h"

// This is used for debug purpose, to draw subdivisions done
std::vector<SDL_Rect> QuadtreeAABBs;

// Insert new point in the quadtree
bool AABB::Insert(iPoint* newpoint)
{
	// The new point is inside the quadtree AABB?
	if (!contains(newpoint))
		return false;
	// Add the point to the node if there is space for it (remember Max_Elements_in_Same_Node)
	if (objects.size() < Max_Elements_in_Same_Node)
	{
		objects.push_back(*newpoint);
		return true;
	}
	// If there is not space, add the point to a subdivision
	if (children[0] == nullptr)
		subdivide();

	for (uint i = 0; i < 4; i++)
		if (children[i]->Insert(newpoint))
			return true;

	return false;
}

// Subdivade this quadtree node into 4 smaller children
void AABB::subdivide()
{
	// Calculate the size and position of each of the 4 new nodes
	iPoint qSize = { (int)(aabb.w * 0.5), (int)(aabb.h*0.5) };
	iPoint qCentre;

	// Now we have all the positions and size of each number we 
	// can create each child with its AABB
	// Don't forget to set child root

	qCentre = { aabb.x, aabb.y };
	children[0] = new AABB({ qCentre.x, qCentre.y, qSize.x, qSize.y });
	children[0]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x, qCentre.y, qSize.x, qSize.y });

	qCentre = { aabb.x + qSize.x, aabb.y };
	children[1] = new AABB({ qCentre.x, qCentre.y, qSize.x, qSize.y });
	children[1]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x, qCentre.y, qSize.x, qSize.y });


	qCentre = { aabb.x, aabb.y+qSize.y };
	children[2] = new AABB({ qCentre.x, qCentre.y, qSize.x, qSize.y });
	children[2]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x, qCentre.y, qSize.x, qSize.y });


	qCentre = { aabb.x + qSize.x, aabb.y + qSize.y };
	children[3] = new AABB({ qCentre.x, qCentre.y, qSize.x, qSize.y });
	children[3]->root = this;
	QuadtreeAABBs.push_back({ qCentre.x, qCentre.y, qSize.x, qSize.y });



}

int AABB::CollectCandidates(std::vector<iPoint>& nodes, const SDL_Rect& r)
{
	uint ret = 0;

	//Check if range is in the quadtree AABB
	if (!AABB(aabb).intersects(&AABB(r)))
		return ret;


	//See if the points of this node are in range, get them
	//(remember Max_Elements_in_Same_Node and ret to count how many points we check)
	if(!objects.empty())
		for (int i = 0; i < Max_Elements_in_Same_Node; i++)
		{
			ret++;
			if (AABB(r).contains(&objects[i]))
				nodes.push_back(objects[i]);
		}

	//If the node don't have children, we can end
	if (children[0] == nullptr)
		return ret;

	//If the noide has children, get them points
	ret += children[0]->CollectCandidates(nodes, r);
	ret += children[1]->CollectCandidates(nodes, r);
	ret += children[2]->CollectCandidates(nodes, r);
	ret += children[3]->CollectCandidates(nodes, r);



	return ret;
}