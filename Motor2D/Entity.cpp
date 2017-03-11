#include "Entity.h"
#include "Animation.h"
#include "j1Render.h"
#include "j1App.h"
#include "p2Defs.h"
#include "j1CollisionManager.h"
#include "j1App.h"

Entity::Entity() :
	sprite(nullptr),
	currentAnim(nullptr),
	actionState(IDLE),
	currentDir(D_DOWN),
	lastPos(iPoint()),
	currentPos(iPoint()),
	col(nullptr),
	colPivot(iPoint()),
	type(ENTITY_TYPE(0)),
	life(1)
{
	anim.clear();
}

Entity::Entity(ENTITY_TYPE type) :
	sprite(nullptr),
	currentAnim(nullptr),
	actionState(IDLE),
	currentDir(D_DOWN),
	lastPos(iPoint()),
	currentPos(iPoint()),
	col(nullptr),
	colPivot(iPoint()),
	type(type),
	life(1)
{
	anim.clear();
}

Entity::~Entity()
{
	RELEASE(sprite);
	RELEASE(currentAnim);

	// TODO: tell collision manager to release the entity's collider
	//App->collisions->

	anim.clear();
}

bool Entity::Draw()
{
	bool ret = true;

	currentAnim = &anim.find({ actionState, currentDir })->second;
	sprite->updateSprite(currentPos, currentAnim->pivot, currentAnim->getCurrentFrame(), currentAnim->flip);
	App->render->Draw(sprite);

	return ret;
}