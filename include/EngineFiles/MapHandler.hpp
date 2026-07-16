#pragma once
#include <string>

namespace MapHandler
{
	void init();

	bool new_map(const std::string& new_map_name);

	template <class Item>
	bool add_item_to_map(const std::string& new_item_name, const std::string& instance = "");

	template <class Item>
	bool remove_item_from_map(const std::string& item_name);

	template <class Item>
	bool is_item_MapHandled(const std::string& item_name);

	const std::string& get_last_error();

	void close();
}
