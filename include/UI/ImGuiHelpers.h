#pragma once
#include <SFML/System/Vector2.hpp>
#include "imgui.h"

namespace ImGui {

	inline bool DragFloat2(const char* label, sf::Vector2f& v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return DragFloat2(label, &v.x, v_speed, v_min, v_max, format, flags);
	}

	inline bool SliderFloat2(const char* label, sf::Vector2f& v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
		return SliderFloat2(label, &v.x, v_min, v_max, format, flags);
	}

}
