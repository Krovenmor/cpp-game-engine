#pragma once

#include "EngineFiles/Saving/JsonSF/Vector2.hpp"


struct TransformableStruct {
	sf::Vector2f position;
	float rotation;
	sf::Vector2f scale;
	sf::Vector2f origin;

	// For dop info
	std::string info;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TransformableStruct,
		position,
		rotation,
		scale,
		origin,
		info);

	void print() const;
};

struct TransformableConfig {
	std::unordered_map<std::string, TransformableStruct> um_structs;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TransformableConfig,
		um_structs);
};