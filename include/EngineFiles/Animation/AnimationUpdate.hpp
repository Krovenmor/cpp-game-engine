#pragma once

namespace sf
{
	class Time;
}

class Animation;
class AnimationComponent;

typedef unsigned long size_t;

namespace AnimationUpdate
{
	size_t add_animation(Animation* comp);
	size_t add_animation_component(AnimationComponent* comp);

	void delete_animation(size_t index);
	void delete_animation_component(size_t index);

	void check_and_update(const sf::Time& elapsed);
}