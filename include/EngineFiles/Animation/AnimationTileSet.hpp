#pragma once
#include "Animation.hpp"

class TileSetContainer;

class AnimationTileSet final : public Animation {
private:

	TileSetContainer* tileset = nullptr;

	bool InitAnimation(sf::Time deltaTime);
	bool InitAnimation(const std::vector<sf::Time>& deltaTimes);

	bool LoadAnimation(const char* tilesetName);

	void onGui() override;

public:
	AnimationTileSet() = default;
	AnimationTileSet(const bool turn_automatic_update) : Animation(turn_automatic_update) {};

	bool LoadAnimation(sf::Time deltaTime, const char* tilesetName);
	bool LoadAnimation(const std::vector<sf::Time>& deltaTimes, const char* tilesetName);

	~AnimationTileSet() override;
};
