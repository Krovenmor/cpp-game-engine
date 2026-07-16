#pragma once
#include <functional>
#include <unordered_map>
#include <SFML/Graphics.hpp>


class InputComponent {
private:

	using Action = std::function<void(bool)>;
	std::unordered_map<sf::Keyboard::Key, Action> binded_keys;

public:

	InputComponent();

	void bind_key(sf::Keyboard::Key key, Action action);

	void deal_with_key(sf::Keyboard::Key& key, bool value);

	void on_event(sf::Event& event);

	// Game Engine func!
	// It's only used by Interface drawing, and better not use it in game code
	void on_gui();

	~InputComponent();

};