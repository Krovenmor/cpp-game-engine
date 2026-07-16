#pragma once
#include <SFML/Graphics.hpp>

#include "BaseClasses/Actor.h"
#include "EngineFiles/CollisionTypes.hpp"


class Actor;

class CollisionComponent {
private:

	// Pointer to Base class
	Actor* actor;
	// Id to get vaCollision from Actor
	int obj_id = -1;

	// Collision
	CollisionType type = CollisionType::Static;
	sf::FloatRect* boxCollision = nullptr;
	bool is_collision_visible = false;
	bool is_on = false;
	
	// Needs for init bounds
	sf::FloatRect calculateInternalBounds() const;

	void initialization_of_collision(sf::FloatRect collision_bounds);
	void initialization_of_collision_painting();

	void re_check_collision_painting() const;

	sf::FloatRect recalc_globalBounds() const;

public:

	CollisionComponent(Actor* actor_parent_class_ptr) : actor(actor_parent_class_ptr) {}

	CollisionComponent(const CollisionComponent&) = delete;
	CollisionComponent& operator=(const CollisionComponent&) = delete;

	// Turning Collision on
	// INFO: you must call this func after adding all objs to Actor, for automatic
	// calculation collision box bounds, and before using any other CollisionComponent funcs
	void TurnOnCollision(CollisionType type_);
	void TurnOffCollision();

	// Resize Collision Box
	void AddToCollisionBoxSize(float x, float y);
	// Set Collision Box size
	void SetCollisionBoxSize(float x, float y);
	//
	void SetCollisionBoxPosition(float x, float y);

	// It's needed for making your collision box visible (DEBUG)
	void make_collision_visible();

	// Game Engine func!
	// It's only used by CollisionManager, and better not use it in game code
	void make_collision_event(CollisionComponent* otherActor);

	// Game Engine func!
	// It's only used by Interface drawing, and better not use it in game code
	void on_gui();

	// Needs to be implemented in child classes
	virtual void on_collision(Actor* otherActor, CollisionComponent* otherCollisionComponent) = 0;

	sf::FloatRect getCollisionBoxBoundsGlobal() const;
	sf::Vector2f getCollisionBoxCenterPositionGlobal() const;
	sf::Vector2f getCollisionBoxLocalSize() const;
	sf::Vector2f getCollisionBoxGlobalSize() const;

	virtual ~CollisionComponent();
};
