#pragma once
#include <MainCycle/MainCycle.h>
#include <BaseClasses/TileMap.h>
#include <CombinedUsefulClasses/TextWidget.h>

class Text final : public TextWidget {
public:
	Text() : TextWidget("WelcomeWidget", Anchor::Center) {
		text->setString("Welcome!");
		text->setCharacterSize(45);
		text->setFillColor(sf::Color::Cyan);
		set_font("materials/Fonts/test.ttf");
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