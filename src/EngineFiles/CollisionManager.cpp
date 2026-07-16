#include "EngineFiles/CollisionManager.hpp"
#include "Components/CollisionComponent.h"


namespace CollisionManager
{
	Logger lCM("CollisionManager.log");

	std::vector<Node> nodes;

	void check_in(CollisionComponent* a, CollisionType type) {
		nodes.push_back({a, a->getCollisionBoxBoundsGlobal(), type});
	}

	auto get_it(CollisionComponent* a) {
		auto it = std::find_if(nodes.begin(), nodes.end(),
			  [a](const Node& n) { return n.obj == a; });
		return it;
	}

	void check_out(CollisionComponent* a) {
		auto it = get_it(a);

		if (it != nodes.end()) {
			if (it + 1 != nodes.end()) {
				*it = std::move(nodes.back());
			}
			nodes.pop_back();
		} else {
			lCM.error() << "check_out: wrong ptr passed!";
		}
	}

	void search() {
		if (nodes.size() < 2)
			return;

		for (auto& node : nodes) {
			if (node.type == CollisionType::Dynamic) node.bounds = node.obj->getCollisionBoxBoundsGlobal();
		}

		std::stable_sort(nodes.begin(), nodes.end(),
			[](const Node& a, const Node& b) {
				return a.bounds.left < b.bounds.left;
			});

		for (size_t i = 0; i < nodes.size(); ++i) {
			const auto& nodeA = nodes[i];
			const float max_x = nodeA.bounds.left + nodeA.bounds.width;

			for (size_t j = i + 1; j < nodes.size(); ++j) {
				const auto& nodeB = nodes[j];

				if (nodeB.bounds.left > max_x) {
					break;
				}

				if (nodeA.bounds.intersects(nodeB.bounds)) {
					nodeB.obj->make_collision_event(nodeA.obj);
				}
			}
		}
	}

	const std::vector<Node>& get_data() {
		return nodes;
	}

	CollisionType get_type(CollisionComponent *a) {
		auto it = get_it(a);

		if (it != nodes.end()) {
			return it->type;
		}

		lCM.error() << "get_type: wrong ptr passed!";
		return CollisionType();
	}

	void update_bounds(CollisionComponent *a, sf::FloatRect bounds) {
		auto it = get_it(a);

		if (it != nodes.end()) {
			it->bounds = bounds;
		} else {
			lCM.error() << "update_bounds: wrong ptr passed!";
		}
	}
}
