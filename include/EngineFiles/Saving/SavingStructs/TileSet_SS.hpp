#pragma once
#include <map>
#include <string>

#include "EngineFiles/Saving/JsonSF/Vector2.hpp"

struct TileSetConfig {
	// key - name, value - path
	std::map<std::string, std::string> tilesets;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TileSetConfig,
		tilesets);
};

struct TileSetStruct {
	std::string texture_path;
	sf::Vector2u tileSize;
	std::vector<std::vector<unsigned>> tilesMappingContainer;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TileSetStruct,
		texture_path,
		tileSize,
		tilesMappingContainer);
};