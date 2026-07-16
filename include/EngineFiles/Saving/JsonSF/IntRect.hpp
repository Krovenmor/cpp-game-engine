#pragma once
#include <SFML/Graphics/Rect.hpp>

#include "json.hpp"


namespace sf
{
	inline void to_json(nlohmann::json& j, const sf::IntRect& ir) {
		j = nlohmann::json{
			        {"x", ir.left},
					{"y", ir.top},
					{"w", ir.width},
					{"h", ir.height}
		};
	}

	inline void from_json(const nlohmann::json& j, sf::IntRect& ir) {
		j.at("x").get_to(ir.left);
		j.at("y").get_to(ir.top);
		j.at("w").get_to(ir.width);
		j.at("h").get_to(ir.height);
	}
}