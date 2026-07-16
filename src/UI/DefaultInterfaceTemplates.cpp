#include "UI/DefaultInterfaceTemplates.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "UI/ImGuiHelpers.h"
#include "Utils/UsefulFunctions.h"
#include "EngineFiles/FilesHandler.hpp"
#include "EngineFiles/ObjectStorage.hpp"


namespace DI
{
	bool default_button_with_text(const char* button_text, const char* text) {
		bool return_value = false;
		if (ImGui::Button(button_text)) {
			return_value = true;
		}
		ImGui::SameLine();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", text);
		return return_value;
	}

	void TooltipWithColoredText(const ImVec4& color, const char* text, const char* info) {
		ImGui::TextColored(color, "%s", text);
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(info);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void HelpMarker(const char* info) {
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(info);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	const ImVec4& RED() {
		static ImVec4 red_color(1,0,0,1);
		return red_color;
	}

	const ImVec4& GREEN() {
		static ImVec4 green_color(0,1,0,1);
		return green_color;
	}

	Begin::Begin(const char* label) {ImGui::Begin(label);};
	Begin::Begin(const char* label, bool& p_open) {ImGui::Begin(label, &p_open);}
	Begin::Begin(const char *label, ImGuiWindowFlags_ flags) {{ImGui::Begin(label, nullptr, flags);}}

	Begin::~Begin() {ImGui::End();}

	DefaultTable2Columns::DefaultTable2Columns(const char *table_name) {
		ImGui::BeginTable(table_name, 2, ImGuiTableFlags_SizingStretchSame);
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("Widget");
	}

	void DefaultTable2Columns::new_row(const char *label) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", label);
		ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
	}

	DefaultTable2Columns::~DefaultTable2Columns() {
		ImGui::EndTable();
	}

	DefaultTable1Column::DefaultTable1Column(const char *table_name) {
		ImGui::BeginTable(table_name, 1, ImGuiTableFlags_SizingStretchSame);
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
	}

	DefaultTable1Column::~DefaultTable1Column() {
		ImGui::EndTable();
	}

	void drawVA(const sf::VertexArray& va, sf::Vector2f size, sf::RenderTexture& targetRT, sf::Texture* tex) {
		if (targetRT.getSize().x != (unsigned)size.x || targetRT.getSize().y != (unsigned)size.y) {
			targetRT.create((unsigned)size.x, (unsigned)size.y);
		}

		targetRT.clear(sf::Color::Transparent);

		if (va.getVertexCount() > 0) {
			sf::FloatRect b = va.getBounds();
			sf::View v(sf::Vector2f(b.left + b.width/2.f, b.top + b.height/2.f), sf::Vector2f(b.width, -b.height));
			targetRT.setView(v);
		}

		sf::RenderStates states;
		if (tex) states.texture = tex;
		targetRT.draw(va, states);
		targetRT.display();

		ImGui::Image(targetRT.getTexture());
	}

	void drawVA(const sf::VertexArray& va, sf::Vector2f size, sf::RenderTexture& targetRT, sf::Texture* tex,
		const sf::View& view) {
		if (targetRT.getSize().x != (unsigned)size.x || targetRT.getSize().y != (unsigned)size.y) {
			targetRT.create((unsigned)size.x, (unsigned)size.y);
		}

		targetRT.clear(sf::Color::Transparent);
		targetRT.setView(view);

		sf::RenderStates states;
		if (tex) states.texture = tex;
		targetRT.draw(va, states);
		targetRT.display();

		ImGui::Image(targetRT.getTexture());
	}

	void print_path(const int max_output_size, const char *label, const std::string &path) {
		if (path.size() > max_output_size) {
			std::string_view path_ = path;
			auto pos = path_.find_first_of('/', path_.size() - max_output_size);
			path_.remove_prefix(pos);
			ImGui::Text(label, (std::string("..") + path_.data()).c_str());
			ImGui::SameLine();
			HelpMarker(path.c_str());
		} else {
			ImGui::Text(label, path.c_str());
		}
	}

	void print_gui_primitive_type(const sf::PrimitiveType type) {
		switch (type) {
		case sf::LineStrip:		ImGui::Text("LineStrip"); break;
		case sf::Lines:			ImGui::Text("Lines"); break;
		case sf::Points:		ImGui::Text("Points"); break;
		case sf::Quads:			ImGui::Text("Quads"); break;
		case sf::Triangles:		ImGui::Text("Triangles"); break;
		case sf::TriangleStrip:	ImGui::Text("TriangleStrip"); break;
		case sf::TriangleFan:	ImGui::Text("TriangleFan"); break;
		}
	}

	bool deal_with_color(sf::Color& color, const char* label) {
		static bool is_color_extended;
		bool r_value = false;

		ImGui::BulletText("%s", label);
		show_color_info(color);
		ImGui::SameLine();
		if (!is_color_extended) {
			if (ImGui::SmallButton("Open Color Redactor")) {
				is_color_extended = true;
			}
		} else {
			if (ImGui::SmallButton("Close Color Redactor")) {
				is_color_extended = false;
			}
		}

		if (is_color_extended) {
			float col[4] = {
				color.r / 255.0f,
				color.g / 255.0f,
				color.b / 255.0f,
				color.a / 255.0f
			};

			if (ImGui::ColorEdit4("##ColorPicker", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
				color.r = static_cast<sf::Uint8>(col[0] * 255.0f);
				color.g = static_cast<sf::Uint8>(col[1] * 255.0f);
				color.b = static_cast<sf::Uint8>(col[2] * 255.0f);
				color.a = static_cast<sf::Uint8>(col[3] * 255.0f);
				r_value = true;
			}
			ImGui::SameLine(); ImGui::Text("Current Color");

			ImGui::Text("Palette:");
			static const ImVec4 palette[8] = {
				ImVec4(1,0,0,1), ImVec4(0,1,0,1), ImVec4(0,0,1,1), ImVec4(1,1,0,1), // Red, Green, Blue, Yellow
				ImVec4(1,0,1,1), ImVec4(0,1,1,1), ImVec4(1,1,1,1), ImVec4(0,0,0,1)  // Pink, Cyan, White, Black
			};

			for (int n = 0; n < 8; n++) {
				ImGui::PushID(n);
				if (ImGui::ColorButton("##palette", palette[n], ImGuiColorEditFlags_NoAlpha)) {
					color.r = (sf::Uint8)(palette[n].x * 255);
					color.g = (sf::Uint8)(palette[n].y * 255);
					color.b = (sf::Uint8)(palette[n].z * 255);
					color.a = (sf::Uint8)(palette[n].w * 255);
					r_value = true;
				}
				if (n < 7 && (n + 1) % 4 != 0) ImGui::SameLine();
				ImGui::PopID();
			}
		}
		return r_value;
	}

	bool deal_with_color(sf::Color& color, const char* label, bool& is_color_extended) {
		bool r_value = false;

		ImGui::BulletText("%s", label);
		show_color_info(color);
		ImGui::SameLine();

		if (!is_color_extended) {
			if (ImGui::SmallButton("Open Color Redactor")) {
				is_color_extended = true;
			}
		} else {
			if (ImGui::SmallButton("Close Color Redactor")) {
				is_color_extended = false;
			}
		}

		if (is_color_extended) {
			float col[4] = {
				color.r / 255.0f,
				color.g / 255.0f,
				color.b / 255.0f,
				color.a / 255.0f
			};

			if (ImGui::ColorEdit4("##ColorPicker", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
				color.r = static_cast<sf::Uint8>(col[0] * 255.0f);
				color.g = static_cast<sf::Uint8>(col[1] * 255.0f);
				color.b = static_cast<sf::Uint8>(col[2] * 255.0f);
				color.a = static_cast<sf::Uint8>(col[3] * 255.0f);
				r_value = true;
			}
			ImGui::SameLine(); ImGui::Text("Current Color");

			ImGui::Text("Palette:");
			static const ImVec4 palette[8] = {
				ImVec4(1,0,0,1), ImVec4(0,1,0,1), ImVec4(0,0,1,1), ImVec4(1,1,0,1), // Red, Green, Blue, Yellow
				ImVec4(1,0,1,1), ImVec4(0,1,1,1), ImVec4(1,1,1,1), ImVec4(0,0,0,1)  // Pink, Cyan, White, Black
			};

			for (int n = 0; n < 8; n++) {
				ImGui::PushID(n);
				if (ImGui::ColorButton("##palette", palette[n], ImGuiColorEditFlags_NoAlpha)) {
					color.r = (sf::Uint8)(palette[n].x * 255);
					color.g = (sf::Uint8)(palette[n].y * 255);
					color.b = (sf::Uint8)(palette[n].z * 255);
					color.a = (sf::Uint8)(palette[n].w * 255);
					r_value = true;
				}
				if (n < 7 && (n + 1) % 4 != 0) ImGui::SameLine();
				ImGui::PopID();
			}
		}
		return r_value;
	}

	sf::Font* deal_with_font(const sf::Font* font, std::string& font_path, sf::RenderTexture& rt) {
		sf::Font* font_return = nullptr;
		ImGui::BulletText("Font:");
		if (!font_path.empty()) {
			print_path(30, "Font path: %s", font_path);
		} else {
			ImGui::Text("No path to font!");
		}

		static bool is_changing = false;
		if (ImGui::Button("Change Font")) {
			is_changing = !is_changing;
		}
		if (is_changing) {
			if (file::select_file(SelectType::Font)) {
				is_changing = false;
				std::string file_path = file::get_selected_file();
				if (!file_path.empty()) {
					auto font_ = ObjectStorage::LoadFont(file_path);

					if (font_) {
						font_return = font_;
						font_path = file_path;
					}
				}
			}
		}

		ImGui::SameLine();
		static bool is_preview;
		if (!is_preview && ImGui::Button("Open preview")) {
			is_preview = true;
		}
		if (is_preview && ImGui::Button("Close preview")) {
			is_preview = false;
		}

		if (is_preview) {
			ImGui::Separator();
			ImGui::Text("Font preview:");
			if (font) {
				if (rt.getSize().x == 0) rt.create(std::min(ImGui::GetContentRegionAvail().x, 300.f), 40);

				rt.clear(sf::Color::Transparent);
				sf::Text sfText("Preview of font", *font, 24);
				sfText.setFillColor(sf::Color::White);
				rt.draw(sfText);
				rt.display();

				ImGui::Image(rt);
			} else {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No font loaded!");
			}
		}

		return font_return;
	}

	sf::Texture* deal_with_texture(const sf::Texture* texture, std::string& texture_path, const float max_width) {
		unsigned texX = 0, texY = 0;
		sf::Texture* texture_return = nullptr;

		if (texture) {
			texX = texture->getSize().x;
			texY = texture->getSize().y;
		}

		ImGui::BulletText("Texture:");
		if (!texture_path.empty()) {
			print_path(30, "Texture path: %s", texture_path);
		} else {
			ImGui::Text("No path to texture!");
		}

		ImGui::Text("Texture info: size.x = %u, size.y = %u", texX, texY);

		static bool is_changing = false;
		if (ImGui::Button("Change Texture")) {
			is_changing = !is_changing;
		}
		if (is_changing) {
			if (file::select_file(SelectType::Texture)) {
				is_changing = false;
				std::string file_path = file::get_selected_file();
				if (!file_path.empty()) {
					auto texture_ = ObjectStorage::LoadTexture(file_path);

					if (texture_) {
						texture_return = texture_;
						texture_path = file_path;
					}
				}
			}
		}

		ImGui::SameLine();
		static bool is_preview;
		if (!is_preview && ImGui::Button("Open preview")) {
			is_preview = true;
		}
		if (is_preview && ImGui::Button("Close preview")) {
			is_preview = false;
		}

		if (is_preview) {
			ImGui::Separator();
			ImGui::BulletText("Texture preview:");
			show_texture_preview(texture, max_width, max_width);
			ImGui::Separator();
		}

		return texture_return;
	}

	void show_texture_info(const sf::Texture* texture, const bool is_preview, const float max_width) {
		if (texture) {
			const unsigned texX = texture->getSize().x, texY = texture->getSize().y;
			bool is_repeated = texture->isRepeated(), is_smooth = texture->isSmooth(), is_srgb = texture->isSrgb();

			ImGui::Text("Texture size: size.x = %u, size.y = %u", texX, texY);
			ImGui::Checkbox("IsRepeated", &is_repeated);
			ImGui::SameLine();
			ImGui::Checkbox("IsSmooth", &is_smooth);
			ImGui::SameLine();
			ImGui::Checkbox("IsSrgb", &is_srgb);
		}
		if (is_preview) {
			ImGui::BulletText("Texture preview:");
			show_texture_preview(texture, max_width, max_width);
		}
	}

	void show_texture_preview(const sf::Texture *texture, const float max_width, const float max_height) {
		if (texture) {

			const auto text_size = texture->getSize();
			const float width = std::min(ImGui::GetContentRegionAvail().x, max_width);
			const float height = std::min(text_size.y * (width / text_size.x), max_height);
			ImGui::Image(*texture, sf::Vector2f(width, height));

		} else {
			ImGui::TextColored(RED(), "No texture loaded!");
		}
	}

	void show_va_info(const sf::VertexArray& va) {
		auto table = DefaultTable2Columns("VertexArrayInfo");

		table.new_row("Bounds Size");
		const auto rect = va.getBounds();
		ImGui::Text("x: %f, y: %f", rect.getSize().x, rect.getSize().y);

		table.new_row("Bounds Pos");
		ImGui::Text("x: %f, y: %f", rect.getPosition().x, rect.getPosition().y);

		table.new_row("Primitive Type");
		print_gui_primitive_type(va.getPrimitiveType());

		table.new_row("Vertex Count");
		const auto vertex_count = va.getVertexCount();
		ImGui::Text("%lu", vertex_count);
	}

	void show_color_info(const sf::Color& color) {
		ImGui::TextColored(ImVec4(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f), "Current Color");
		ImGui::Text("r: %u, g: %u, b: %u, a: %u", color.r, color.g, color.b, color.a);
	}

	int StringCallback(ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* str = (std::string*)data->UserData;
			str->resize(data->BufTextLen);
			data->Buf = (char*)str->c_str();
		}
		return 0;
	}

	bool input_text(std::string &text_string) {
		int line_count = 1;
		for (char c : text_string) if (c == '\n') line_count++;

		float height = std::clamp(
			(line_count * ImGui::GetTextLineHeight()) + ImGui::GetStyle().FramePadding.y * 2.0f,
			ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f,
			300.0f
		);

		return ImGui::InputTextMultiline("##source",
			(char*)text_string.c_str(),
			text_string.capacity() + 1,
			ImVec2(-1.0f, height),
			ImGuiInputTextFlags_CallbackResize,
			StringCallback,
			(void*)&text_string
		);
	}

	bool is_save_sprite = false;
	bool get_is_save_sprite() {
		return is_save_sprite;
	}

	bool deal_with_sprite(sf::Sprite* sprite_ptr, std::string& new_texture_path) {
		ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_FirstUseEver);
		auto b = Begin("Sprite Inspector", ImGuiWindowFlags_AlwaysAutoResize);

		static bool init = true;
		static sf::Sprite prev_state;

		if (init) {
			prev_state = *sprite_ptr;
			is_save_sprite = false;
			init = false;
		}

		sf::Color new_color = sprite_ptr->getColor();
		if (deal_with_color(new_color, "Sprite Color")) {
			sprite_ptr->setColor(new_color);
		}
		ImGui::Separator();

		if (sf::Texture* texture_return = deal_with_texture(sprite_ptr->getTexture(), new_texture_path)) {
			sprite_ptr->setTexture(*texture_return);
			sprite_ptr->setColor(sf::Color::White);
		}
		ImGui::Separator();

		ImGui::BulletText("Sprite Settings:");
	    if (ImGui::BeginTable("SpriteSettings", 2, ImGuiTableFlags_None)) {
	        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
	        ImGui::TableSetupColumn("Widget", ImGuiTableColumnFlags_WidthStretch);

	    	auto new_row = [](const char* label) {
	    		ImGui::TableNextRow();
	    		ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", label);
	    		ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
	    	};

	    	auto sprite_rect = sprite_ptr->getTextureRect();
	    	sf::Vector2i positionRect = sprite_rect.getPosition();
	    	sf::Vector2i sizeRect = sprite_rect.getSize();

	    	new_row("TextRect Pos");
	    	ImGui::DragInt2("##positionRect", &positionRect.x, 1);

	    	new_row("TextRect Size");
	    	ImGui::DragInt2("##sizeRect", &sizeRect.x, 1);

	    	sf::IntRect new_rect(positionRect, sizeRect);
	    	if (sprite_rect != new_rect) {
	    		sprite_ptr->setTextureRect(new_rect);
	    	}

	    	new_row("Origin");
	    	sf::Vector2f origin = sprite_ptr->getOrigin();
	    	if (ImGui::DragFloat2("##sprite_origin", origin)) {
	    		sprite_ptr->setOrigin(origin);
	    	}

	    	new_row("Position");
	    	sf::Vector2f position = sprite_ptr->getPosition();
	    	if (ImGui::DragFloat2("##position", position)) {
	    		sprite_ptr->setPosition(position);
	    	}

	    	new_row("Scale");
	    	sf::Vector2f scale = sprite_ptr->getScale();
	    	if (ImGui::DragFloat2("##scale", scale, 0.01f)) {
	    		sprite_ptr->setScale(scale);
	    	}

	    	new_row("Rotation");
	    	float rotation = sprite_ptr->getRotation();
	    	if (ImGui::DragFloat("##rotation", &rotation)) {
	    		sprite_ptr->setRotation(rotation);
	    	}

	        ImGui::EndTable();
	    }

		if (ImGui::Button("Save and Close", ImVec2(-FLT_MIN, 30))) {
			is_save_sprite = true;
			init = true;
			return true;
		}
		if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
			*sprite_ptr = prev_state;
			init = true;
			return true;
		}
		return false;
	}

	bool is_save_text = false;
	bool get_is_save_text() {
		return is_save_text;
	}

	bool deal_with_text(sf::Text *text_ptr, std::string& new_font_path) {
	    ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_FirstUseEver);
		auto b = Begin("Text Inspector", ImGuiWindowFlags_AlwaysAutoResize);

		static bool init = true;
		static sf::Text prev_state;
		static bool edit_outline;

		if (init) {
			prev_state = *text_ptr;
			edit_outline = false;
			is_save_text = false;
			init = false;
		}

		ImGui::Checkbox("Outline Mode", &edit_outline);
	    const char* color_label = edit_outline ? "Outline Color:" : "Fill Color:";
	    sf::Color current_color = edit_outline ? text_ptr->getOutlineColor() : text_ptr->getFillColor();

	    if (deal_with_color(current_color, color_label)) {
	        if (edit_outline) text_ptr->setOutlineColor(current_color);
	        else text_ptr->setFillColor(current_color);
	    }
		ImGui::Separator();

		static sf::RenderTexture rt;
		if (auto new_font = deal_with_font(text_ptr->getFont(), new_font_path, rt)) {
			text_ptr->setFont(*new_font);
		}
		ImGui::Separator();

		ImGui::BulletText("TextSettings:");
	    if (ImGui::BeginTable("TextSettings", 2, ImGuiTableFlags_SizingStretchSame)) {
	        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
	        ImGui::TableSetupColumn("Widget");

	    	auto new_row = [](const char* label) {
	    		ImGui::TableNextRow();
	    		ImGui::TableSetColumnIndex(0); ImGui::AlignTextToFramePadding(); ImGui::Text("%s", label);
	    		ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(-FLT_MIN);
	    	};

	    	new_row("Outline Thickness");
	        float thickness = text_ptr->getOutlineThickness();
	        if (ImGui::DragFloat("##thickness", &thickness, 0.1f, 0.0f, 20.0f, "%.1f")) {
	            text_ptr->setOutlineThickness(thickness);
	        }

	    	new_row("Character Size");
	        int size = static_cast<int>(text_ptr->getCharacterSize());
	        if (ImGui::DragInt("##size", &size, 1, 5, 120)) {
	            text_ptr->setCharacterSize(static_cast<unsigned int>(size));
	        }

	    	new_row("Letter Spacing");
	    	float letter_spacing = text_ptr->getLetterSpacing();
	    	if (ImGui::DragFloat("##letter_spacing", &letter_spacing, 0.1f, -100.f, 100.f)) {
	    		text_ptr->setLetterSpacing(letter_spacing);
	    	}

	    	new_row("Line Spacing");
	    	float line_spacing = text_ptr->getLineSpacing();
	    	if (ImGui::DragFloat("##line_spacing", &line_spacing, 0.1f, -100.f, 100.f)) {
	    		text_ptr->setLineSpacing(line_spacing);
	    	}

	    	new_row("Text Origin");
	    	sf::Vector2f origin = text_ptr->getOrigin();
	    	if (ImGui::DragFloat2("##origin", origin, 1.f)) {
	    		text_ptr->setOrigin(origin);
	    	}

	    	new_row("Text Position");
	    	sf::Vector2f position = text_ptr->getPosition();
	    	if (ImGui::DragFloat2("##position", position, 1.f)) {
	    		text_ptr->setPosition(position);
	    	}

	    	new_row("Text Scale");
	    	sf::Vector2f scale = text_ptr->getScale();
	    	if (ImGui::DragFloat2("##scale", scale, 0.01f)) {
	    		text_ptr->setScale(scale);
	    	}

	    	new_row("Text Rotation");
	    	float rotation = text_ptr->getRotation();
	    	if (ImGui::DragFloat("##rotation", &rotation, 1.f)) {
	    		text_ptr->setRotation(rotation);
	    	}

	        ImGui::EndTable();
	    }

	    ImGui::Separator();
	    ImGui::Spacing();

		ImGui::BulletText("Text Style:");
		int style = static_cast<int>(text_ptr->getStyle());
		if (style == 0) {
			ImGui::SameLine();
			ImGui::Text("It's currently regular");
		}
		ImGui::CheckboxFlags("Bold", &style, sf::Text::Bold);
		ImGui::SameLine();
		ImGui::CheckboxFlags("Italic", &style, sf::Text::Italic);
		ImGui::SameLine();
		ImGui::CheckboxFlags("Underlined", &style, sf::Text::Underlined);
		ImGui::SameLine();
		ImGui::CheckboxFlags("StrikeThrough", &style, sf::Text::StrikeThrough);
		if (style != static_cast<int>(text_ptr->getStyle())) {
			text_ptr->setStyle(style);
		}
		ImGui::Spacing();
		ImGui::Separator();

		ImGui::Spacing();
		ImGui::BulletText("String:");
		std::string text = text_ptr->getString();
		if (input_text(text)) {
			text_ptr->setString(text);
		}
		ImGui::Spacing();
		ImGui::Separator();

	    if (ImGui::Button("Save and Close", ImVec2(-FLT_MIN, 30))) {
	    	is_save_text = true;
	    	init = true;
	        return true;
	    }
		if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
			*text_ptr = prev_state;
			init = true;
			return true;
		}
	    return false;
	}

	bool defaultMapSelector(const std::map<std::string, std::string>& map, const char* label,
		const char* column1_name, const char* column2_name) {
		bool result = false;
		const float height = 200.f;
		const float max_width = 800.f;
		float width = ImGui::GetContentRegionAvail().x;
		width = std::min(width, max_width);

		ImGui::Separator();
		ImGui::BulletText("%s", label);
		if (ImGui::BeginTable("SelectionTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
			| ImGuiTableFlags_ScrollY, ImVec2(width, height))) {

			ImGui::TableSetupColumn(column1_name);
			ImGui::TableSetupColumn(column2_name);
			ImGui::TableHeadersRow();

			for (auto& [name, data] : map) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);

				bool is_selected = (map_selected_name == name);

				if (ImGui::Selectable(name.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
					map_selected_name = name;
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%s", data.c_str());
			}
			ImGui::EndTable();
		}

		ImGui::Separator();

		if (!map_selected_name.empty()) {
			ImGui::Text("Currently selected: %s", map_selected_name.c_str());
			if (ImGui::Button("Confirm Selection", ImVec2(-FLT_MIN, 0))) {
				if (!map_selected_name.empty()) {
					result = true;
				}
			}
		} else {
			ImGui::TextDisabled("Please select an item from the list.");
		}
		return result;
	}

	bool defaultVectorSelector(const std::vector<std::string>& vector, const char* label, const char* column_name) {
		bool result = false;
		const float height = 200.f;
		const float max_width = 800.f;
		float width = ImGui::GetContentRegionAvail().x;
		width = std::min(width, max_width);

		ImGui::Separator();
		ImGui::BulletText("%s", label);
		if (ImGui::BeginTable("SelectionTable", 1, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
			| ImGuiTableFlags_ScrollY, ImVec2(width, height))) {

			ImGui::TableSetupColumn(column_name);
			ImGui::TableHeadersRow();

			for (auto& name : vector) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);

				bool is_selected = (vector_selected_name == name);

				if (ImGui::Selectable(name.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
					vector_selected_name = name;
				}
			}
			ImGui::EndTable();
			}

		ImGui::Separator();

		if (!vector_selected_name.empty()) {
			ImGui::Text("Currently selected: %s", vector_selected_name.c_str());
			if (ImGui::Button("Confirm Selection", ImVec2(-FLT_MIN, 0))) {
				if (!vector_selected_name.empty()) {
					result = true;
				}
			}
		} else {
			ImGui::TextDisabled("Please select an item from the list.");
		}
		return result;
	}
}
