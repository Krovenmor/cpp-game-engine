#pragma once
#include "json.hpp"

struct UIConfig {
	bool is_create_menu = false;
	bool is_redactor = false;
	bool is_output = false;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(UIConfig,
		is_create_menu,
		is_redactor,
		is_output);
};
