#include "default.h"

void Main::init(sf::RenderWindow &window)
{
	tm.load();
	tm.scale(3.5f, 3.5f);

	text.set_default_position(window.getSize().x, window.getSize().y);
}

void Main::game_tick(sf::Time elapsed, sf::RenderWindow &window)
{

}

void Main::on_event(sf::Event &event, sf::RenderWindow &window)
{

}