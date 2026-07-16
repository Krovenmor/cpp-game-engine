#include <set>
#include "Utils/GameTools.hpp"

namespace spawn
{
	storage spawnables;
	std::set<SpawnableComponent*> to_delete;

	storage& get_spawnables() {
		return spawnables;
	}

	void check() {
		for (auto* obj : to_delete) {
			size_t currentIndex = obj->get_index_sp();

			if (currentIndex < spawnables.size() && spawnables[currentIndex].get() == obj) {
				if (currentIndex != spawnables.size() - 1) {
					spawnables[currentIndex] = std::move(spawnables.back());
					spawnables[currentIndex]->set_index(currentIndex);
				}
				spawnables.pop_back();
			}
		}
		to_delete.clear();
	}

	void kill_spawnable(SpawnableComponent* comp) {
		to_delete.insert(comp);
	}
}
