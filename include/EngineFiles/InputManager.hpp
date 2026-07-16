#pragma once

namespace sf
{
	class Event;
}

class InputComponent;

namespace InputManager
{
	void on_event(sf::Event& event);

	void check_in(InputComponent* ic);

	void check_out(InputComponent* ic);
}