#pragma once
#include <SFML/Graphics/Color.hpp>

#include "json.hpp"


namespace sf {
	inline void to_json(nlohmann::json& j, const sf::Color& c) {
		j = nlohmann::json{
				        {"r", c.r},
						{"g", c.g},
						{"b", c.b},
						{"a", c.a}
		};
	}

	inline void from_json(const nlohmann::json& j, sf::Color& c) {
		j.at("r").get_to(c.r);
		j.at("g").get_to(c.g);
		j.at("b").get_to(c.b);
		j.at("a").get_to(c.a);
	}
}