#pragma once
#include <MainCycle/MainCycle.h>
#include <Utils/TileMap.h>
#include <CombinedUsefulClasses/TextWidget.h>

class Text final : public TextWidget {
public:
	Text() : TextWidget(Anchor::Center) {
		set_name("Welcome!");
		set_font("materials/Fonts/test.ttf");
		get_text().setCharacterSize(45);
		get_text().setString(get_name());
		get_text().setFillColor(sf::Color::Cyan);
	}
};

struct Main final : game {

	TileMap tm = TileMap("tilemap_test");
	Text text;

	// Function will be executed before game_loop
	void init(sf::RenderWindow& window) override;

	// Function will be executed every loop in game_loop
	void game_tick(sf::Time elapsed, sf::RenderWindow& window) override;

	// Calls when event happened
	void on_event(sf::Event& event, sf::RenderWindow& window) override;
};