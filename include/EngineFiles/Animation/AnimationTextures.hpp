#pragma once
#include "Animation.hpp"

class AnimationTextures final: public Animation {
private:

	sf::VertexArray make_va_frame_texture(const sf::Texture* texture);

	void onGui() override;

public:

	AnimationTextures() = default;
	AnimationTextures(const bool turn_automatic_update) : Animation(turn_automatic_update) {};

	bool MakeAnimation(sf::Time deltaTime, const std::vector<std::string>& texture_paths);
	bool MakeAnimation(const std::vector<sf::Time>& deltaTimes, const std::vector<std::string>& texture_paths);

	~AnimationTextures() override = default;
};