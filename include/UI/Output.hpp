#pragma once
#include <string>

namespace Output
{
	void error(const char* error);
	void error(const std::string& error);
	void entry(const char* msg);
	void entry(const std::string& msg);

	void set_is_collapsed(bool is_collapsed_);
	bool get_is_collapsed();
	void draw();

	void on_error(std::string&& s);
	void on_entry(std::string&& s);

	template <typename T>
	void error(const T& msg) {
		on_error(std::to_string(msg));
	}

	template <typename T>
	void entry(const T& msg) {
		on_entry(std::to_string(msg));
	}
}