#pragma once
#include <memory>
#include <vector>
#include <SFML/System/Time.hpp>

#include "Components/SpawnableComponent.hpp"

namespace spawn
{
	using storage = std::vector<std::unique_ptr<SpawnableComponent>>;

	storage& get_spawnables();

	template <class Spawnable, typename ... Args>
	void spawn(Args... args) {
		static_assert(std::is_base_of_v<SpawnableComponent, Spawnable>,
			  "Spawnable is not derived from SpawnableComponent!");
		auto& spawnables = get_spawnables();
		spawnables.emplace_back(std::make_unique<Spawnable>(args...));
		spawnables.back()->set_index(spawnables.size() - 1);
	}

	void kill_spawnable(SpawnableComponent* comp);

	void check();
}

namespace data
{
	inline sf::Time elapsedTime;
}