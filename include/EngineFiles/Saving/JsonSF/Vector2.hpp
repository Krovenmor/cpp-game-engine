#pragma once
#include <SFML/System/Vector2.hpp>

#include "json.hpp"


namespace sf
{
	template<typename A>
	void to_json(nlohmann::json& j, const Vector2<A>& v) {
		j = nlohmann::json{{"x", v.x}, {"y", v.y}};
	}

	template<typename A>
	void from_json(const nlohmann::json& j, sf::Vector2<A>& v) {
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
	}
}
