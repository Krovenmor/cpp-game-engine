#include "EngineFiles/InputManager.hpp"

#include "Components/InputComponent.h"
#include "Utils/UsefulFunctions.h"

namespace InputManager
{
	std::vector<InputComponent*> input_components;

	void on_event(sf::Event& event) {
		for (InputComponent* component : input_components) {
			component->on_event(event);
		}
	}

	void check_in(InputComponent* ic) {
		input_components.push_back(ic);
	}

	void check_out(InputComponent* ic) {
		fast_remove(input_components, ic);
	}
}