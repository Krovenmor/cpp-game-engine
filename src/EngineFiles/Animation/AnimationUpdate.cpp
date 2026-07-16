#include "EngineFiles/Animation/AnimationUpdate.hpp"
#include "EngineFiles/Animation/Animation.hpp"

#include "Components/AnimationComponent.h"

#include "Utils/FastVector.hpp"


namespace AnimationUpdate
{
	FastVector<Animation*> objects;
	FastVector<AnimationComponent*> components;

	size_t add_animation(Animation* comp) {
		return objects.push(comp);
	}

	size_t add_animation_component(AnimationComponent *comp) {
		return components.push(comp);
	}

	void delete_animation(size_t index) {
		objects.erase(index);
	}

	void delete_animation_component(size_t index) {
		components.erase_anim(index);
	}

	void check_and_update(const sf::Time& elapsed) {
		for (const auto& obj : objects) {
			if (obj) {
				obj->check_and_update(elapsed);
			} else {
				std::cerr << "AnimationUpdate, check_and_update: false object in objects!!!\n";
			}
		}

		for (const auto& comp : components) {
			if (comp) {
				comp->check_and_update(elapsed);
			} else {
				std::cerr << "AnimationUpdate, check_and_update: false comp in components!!!\n";
			}
		}
	}
}