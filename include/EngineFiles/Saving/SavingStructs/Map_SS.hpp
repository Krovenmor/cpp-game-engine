#pragma once
#include <map>
#include <string>
#include <vector>

#include "json.hpp"

struct MapsConfig {
	// key - name, data - path to map_data.json
	using map_type = std::map<std::string, std::string>;

	std::string default_map;
	map_type maps_paths;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MapsConfig,
		default_map,
		maps_paths);

	MapsConfig() = default;
	MapsConfig(const std::string& default_map, map_type&& map) : default_map(default_map), maps_paths(map) {}
};

struct MapStruct {
	std::string map_name;
	// .first - number of entities to spawn, .second - name of actor to spawn
	//std::vector<std::pair<unsigned, std::string>> actors;

	// Names of tileMap to spawn
	std::vector<std::string> tileMaps;
	//
	// .first - Instance of Actor, .second - Names of Actors to spawn
	std::vector<std::pair<std::string, std::string>> Actors;


	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MapStruct,
		map_name,
		tileMaps,
		Actors);

	MapStruct() = default;
	MapStruct(const std::string& map_name) : map_name(map_name) {}
};