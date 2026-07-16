#pragma once
#include <string>

enum class SelectType {
	Texture,
	Font
};

namespace file
{
	bool select_file(SelectType type);

	std::string get_selected_file();

	bool delete_file(const std::string& filePath);
}
