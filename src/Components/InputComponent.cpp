#include "Components/InputComponent.h"

#include "EngineFiles/InputManager.hpp"
#include "UI/DefaultInterfaceTemplates.hpp"
#include "Utils/UsefulFunctions.h"

InputComponent::InputComponent() {InputManager::check_in(this);};

void InputComponent::bind_key(const sf::Keyboard::Key key, Action action) {
	binded_keys[key] = std::move(action);
}

void InputComponent::deal_with_key(sf::Keyboard::Key& key, bool value) {
	auto it = binded_keys.find(key);
	if (it != binded_keys.end()) {
		it->second(value);
	}
}

void InputComponent::on_event(sf::Event& event)  {
	switch (event.type) {
	case sf::Event::KeyPressed: {
		deal_with_key(event.key.code, true);
		break;
	}
	case sf::Event::KeyReleased: {
		deal_with_key(event.key.code, false);
		break;
	}
	default:
		break;
	}
}

void InputComponent::on_gui() {
	ImGui::BulletText("Input Component info:");
	ImGui::Text("Binded keys:");
	auto tb = DI::DefaultTable2Columns("InputComponent");
	int counter = 0;
	for (auto& [key, action] : binded_keys) {
		counter++;
		char c = '~';

		if (key >= sf::Keyboard::A && key <= sf::Keyboard::Z) {
			c = static_cast<char>(key + 'a');
		}
		if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
			c = static_cast<char>(key - sf::Keyboard::Num0 + '0');
		}

		std::string bindedKey = ("Binded key: " + std::to_string(counter));
		tb.new_row((bindedKey.c_str()));
		ImGui::Text(("%c"), c);
	}
}

InputComponent::~InputComponent() {InputManager::check_out(this);};