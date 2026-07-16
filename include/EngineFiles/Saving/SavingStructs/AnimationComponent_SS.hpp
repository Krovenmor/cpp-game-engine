#pragma once
#include <SFML/System/Time.hpp>

#include "EngineFiles/Saving/JsonSF/Time.hpp"

#define CONFIG_AC_PATH "assets/config/ac_config.json"
#define AC_DATA_PATH "assets/data/"


struct AnimationTileSetData {
	std::string tilesetName;
	std::vector<sf::Time> dTimes;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnimationTileSetData,
		tilesetName,
		dTimes)
};

struct AnimationComponentConfig {

	// key - anim_name, value - data
	std::map<std::string, AnimationTileSetData> tilesets_data;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnimationComponentConfig,
		tilesets_data)
};