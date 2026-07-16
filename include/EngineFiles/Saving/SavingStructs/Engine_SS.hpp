#pragma once
#include "json.hpp"

struct GameEngineConfig {
	unsigned antialiasingLevel = 4;
	unsigned resX = 800;
	unsigned resY = 800;
	std::string window_title = "SFML Game";
	bool is_vertical_sync_enabled = false;
	unsigned framerate_limit = 60;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameEngineConfig,
		antialiasingLevel,
		resX, resY,
		window_title,
		is_vertical_sync_enabled,
		framerate_limit);

	void print() const;
};
