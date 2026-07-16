#pragma once
#include <vector>
#include <SFML/Graphics/Rect.hpp>

#include "CollisionTypes.hpp"

class CollisionComponent;

namespace CollisionManager
{
	struct Node {
		CollisionComponent* obj;
		sf::FloatRect bounds;
		CollisionType type;
	};

	void check_in(CollisionComponent* a, CollisionType type);

	void check_out(CollisionComponent* a);

	void search();

	const std::vector<Node>& get_data();

	CollisionType get_type(CollisionComponent* a);

	void update_bounds(CollisionComponent* a, sf::FloatRect bounds);
}