#pragma once
#include <SFML/Graphics.hpp>
#include "EngineFiles/SystemFactory.hpp"

// For game
struct game {
	virtual ~game() = default;
	virtual void init(sf::RenderWindow& window) = 0;
	virtual void game_tick(sf::Time elapsed, sf::RenderWindow& window) = 0;
	virtual void on_event(sf::Event& event, sf::RenderWindow& window) = 0;
};

int MainCycle(game& currentGame);