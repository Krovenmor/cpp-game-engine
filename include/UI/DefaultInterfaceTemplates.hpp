#pragma once
#include <map>
#include <string>
#include <vector>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/System/Vector2.hpp>

#include "imgui.h"

namespace sf
{
	class View;
	class VertexArray;
	class Font;
	class RenderTexture;
	class Text;
	class Texture;
	class Color;
	class Sprite;
}

namespace DI
{

	bool default_button_with_text(const char* button_text, const char* text);

	void TooltipWithColoredText(const ImVec4& color, const char* text, const char* info);
	void HelpMarker(const char* info);

	const ImVec4& RED();
	const ImVec4& GREEN();

	struct Begin {
		explicit Begin(const char* label);
		explicit Begin(const char* label, bool& p_open);
		explicit Begin(const char* label, ImGuiWindowFlags_ flags);
		~Begin();
	};

	struct DefaultTable2Columns {
		explicit DefaultTable2Columns(const char* table_name);
		void new_row(const char* label);
		~DefaultTable2Columns();
	};

	struct DefaultTable1Column {
		explicit DefaultTable1Column(const char* table_name);
		~DefaultTable1Column();
	};

	void drawVA(const sf::VertexArray& va, sf::Vector2f size, sf::RenderTexture& targetRT, sf::Texture* tex);

	void drawVA(const sf::VertexArray& va, sf::Vector2f size, sf::RenderTexture& targetRT, sf::Texture* tex, const sf::View& view);

	// Label must contain one %s
	void print_path(int max_output_size, const char* label, const std::string& path);

	//
	void print_gui_primitive_type(sf::PrimitiveType type);

	// Returns true when done
	bool deal_with_color(sf::Color& color, const char* label);

	// Returns true when done, write is_color_extended value in given bool&
	bool deal_with_color(sf::Color& color, const char* label, bool& is_color_extended);

	// On Loading new font: Returns valuable ( not nullptr ) ptr, and write new font_path
	sf::Font* deal_with_font(const sf::Font* font, std::string& font_path, sf::RenderTexture& rt);

	// On Loading new texture: Returns valuable ( not nullptr ) ptr, and write new texture_path
	sf::Texture* deal_with_texture(const sf::Texture* texture, std::string& texture_path, float max_width = 300.f);

	//
	void show_texture_info(const sf::Texture* texture, bool is_preview, float max_width = 300.f);

	//
	void show_texture_preview(const sf::Texture* texture, float max_width = 300.f, float max_height = 300.f);

	//
	void show_va_info(const sf::VertexArray& va);

	//
	void show_color_info(const sf::Color& color);

	// Returns true on changing values
	bool input_text(const std::string& text_string);

	// For tracking just close or save
	bool get_is_save_sprite();
	// Returns true when done
	bool deal_with_sprite(sf::Sprite* sprite_ptr, std::string& new_texture_path);

	// For tracking just close or save
	bool get_is_save_text();
	// Returns true when done
	bool deal_with_text(sf::Text* text_ptr, std::string& new_font_path);

	inline std::string map_selected_name;
	bool defaultMapSelector(const std::map<std::string, std::string>& map, const char* label,
		const char* column1_name, const char* column2_name);

	inline std::string vector_selected_name;
	bool defaultVectorSelector(const std::vector<std::string>& vector, const char* label,
		const char* column_name);
}
