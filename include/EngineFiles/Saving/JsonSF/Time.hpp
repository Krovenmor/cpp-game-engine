#pragma once
#include <SFML/System/Time.hpp>

#include "json.hpp"


namespace sf
{
	inline void to_json(nlohmann::json& j, const sf::Time& t) {
		j = t.asSeconds();
	}

	inline void from_json(const nlohmann::json& j, sf::Time& t) {
		t = sf::seconds(j.get<float>());
	}
}