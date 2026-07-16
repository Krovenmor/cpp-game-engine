#pragma once

#include "EngineFiles/Saving/JsonSF/Color.hpp"
#include "EngineFiles/Saving/JsonSF/IntRect.hpp"
#include "EngineFiles/Saving/JsonSF/Vector2.hpp"

#define CONFIG_ACTOR_PATH "assets/config/actors_config.json"

namespace sf
{
	class Sprite;
	class Text;
}

struct SpriteSettings {
	sf::Color color;
	std::string texture_path;
	sf::IntRect rect;
	sf::Vector2f origin;
	sf::Vector2f scale = {1.0f, 1.0f};
	sf::Vector2f position;
	float rotation = 0.f;

	void apply(sf::Sprite* sprite_ptr);
	void save(sf::Sprite* sprite_ptr);

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpriteSettings,
		color,
		texture_path,
		rect,
		origin,
		scale,
		position,
		rotation)
};

struct TextSettings {
	sf::Color outline_color;
	sf::Color fill_color;
	std::string text;
	std::string font_path;
	float thickness;
	unsigned size;
	float letter_spacing;
	float line_spacing;
	sf::Vector2f origin;
	sf::Vector2f scale = {1.0f, 1.0f};
	sf::Vector2f position;
	float rotation = 0.f;
	unsigned style;

	void apply(sf::Text* text_ptr);
	void save(sf::Text* text_ptr);

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(TextSettings,
		outline_color,
		fill_color,
		font_path,
		text,
		thickness,
		size,
		letter_spacing,
		line_spacing,
		origin,
		scale,
		position,
		rotation,
		style)
};

struct ActorData {
	// key - sprite name
	std::map<std::string, SpriteSettings> sprites_settings;
	// key - text name
	std::map<std::string, TextSettings> texts_settings;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ActorData,
		sprites_settings,
		texts_settings)
};

struct ActorConfig {
	// name and path to data
	std::map<std::string, std::string> actors;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ActorConfig,
		actors);
};