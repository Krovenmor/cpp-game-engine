#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <SFML/System/Time.hpp>

#include "EngineFiles/Saving/SavingComponent.hpp"

class Actor;
class Animation;
struct AnimationComponentConfig;

enum class AnimationType {
	// Means it won't call callback() func
	Looping,
	// When animation fully played, callback() func will be called
	One_shot
};

class AnimationComponent {
private:

	using func = std::function<int()>;

	Actor* actor_;
	size_t AnimComponentIndex = 0;

	SavingComponent<AnimationComponentConfig, AnimationComponent>* saving_component = nullptr;
	void init_saving();

	struct AnimStruct {
		int animation_index;
		AnimationType type;
		sf::Time duration;
		func* callback_ptr = nullptr;
	};

	sf::Time current_animation_playtime;
	size_t current_animation_index = 0;
	Animation* current_animation = nullptr;
	std::vector<AnimStruct*> animations;

	void check_and_switch();

public:

	explicit AnimationComponent(Actor* actor_ptr);

	void AnnounceAnimation(const char* animation_name, AnimationType type, func* callback = nullptr);

	void AddAnimation(std::unique_ptr<Animation>&& animation, AnimationType type, func* callback = nullptr);

	bool AddAnimationTileSet(sf::Time deltaTime, const char* tilesetName, AnimationType type, func* callback = nullptr);

	bool AddAnimationTextures(sf::Time deltaTime, const std::vector<std::string>& texture_paths
		, AnimationType type, func* callback = nullptr);

	void switch_animation(size_t index);

	size_t get_animations_size() const {
		return animations.size();
	}

	size_t get_prev_animation() const {
		if (animations.size() == 1) return 0;

		size_t index = (current_animation_index == 0) ? animations.size() - 1 : current_animation_index - 1;
		return index;
	}

	size_t get_next_animation() const {
		if (animations.size() == 1) return 0;

		size_t index = (current_animation_index + 1 >= animations.size()) ? 0 : current_animation_index + 1;
		return index;
	}

	size_t get_current_animation() const {
		return current_animation_index;
	}

	void on_gui();
	void on_animIndex_change(size_t index);
	void check_and_update(sf::Time elapsed);

	~AnimationComponent();
};