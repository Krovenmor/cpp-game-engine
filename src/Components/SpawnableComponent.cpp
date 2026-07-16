#include "Components/SpawnableComponent.hpp"
#include "Utils/GameTools.hpp"

void SpawnableComponent::die() {
	spawn::kill_spawnable(this);
}
