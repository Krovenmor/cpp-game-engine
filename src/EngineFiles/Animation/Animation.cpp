#include <SFML/Graphics/RenderTarget.hpp>
#include "EngineFiles/Animation/Animation.hpp"

#include <SFML/Graphics/RenderTexture.hpp>

#include "EngineFiles/Animation/AnimationUpdate.hpp"
#include "UI/DefaultInterfaceTemplates.hpp"

#include "Utils/Logger.h"

#include "imgui.h"
#include "imgui-SFML.h"

Logger Animation::lA("Animation.log");

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	if (!vAnimFrames.empty()) {
		const AnimFrame& obj = vAnimFrames[stCurrentObj];
		states.texture = obj.texture;
		target.draw(obj.va, states);
	} else {
		lA.error() << "void draw(), vAnimFrames is null, "
			<< "use MakeAnimation_... to add objs to draw!!!" << std::endl;
	}
}

Animation::Animation() : TransformableSaved("Animation", "Animation_transformations", this) {
	obj_index = AnimationUpdate::add_animation(this);
	is_updating = true;
}

Animation::Animation(const bool turn_automatic_update) : TransformableSaved("Animation", "Animation_transformations", this) {
	if (turn_automatic_update) {
		obj_index = AnimationUpdate::add_animation(this);
		is_updating = true;
	}
}

void Animation::check_and_update(const sf::Time& elapsed) {
	if (!is_playing) return;

	if (vAnimFrames.size() > stCurrentObj) {
		if (playbackSpeed != 1.f) {
			const float seconds = elapsed.asSeconds() * playbackSpeed;
			elapsedTime += sf::Time(sf::seconds(seconds));
		} else {
			elapsedTime += elapsed;
		}
		if (elapsedTime >= vAnimFrames[stCurrentObj].dTime) {
			elapsedTime -= vAnimFrames[stCurrentObj].dTime;
			if (stCurrentObj + 1 < vAnimFrames.size()) {
				stCurrentObj++;
			}
			else {
				elapsedTime = sf::Time::Zero;
				stCurrentObj = 0;
			}
		}
	} else {
		lA.error() << "check_and_update(const sf::Time& elapsed): "
			<< "Trouble checking for animation frame, AnimFrame id:" << stCurrentObj;
	}
}

void Animation::turn_update(const bool value) {
	if (value && !is_updating) {
		obj_index = AnimationUpdate::add_animation(this);
		is_updating = value;
	} else if (!value && is_updating) {
		AnimationUpdate::delete_animation(obj_index);
		is_updating = value;
	}
}

void Animation::set_playback_speed(float speed) {
	if (speed <= 0.001f) {
		speed = 0.001f;
	}
	playbackSpeed = speed;
}

void Animation::pause_playback() {
	is_playing = false;
	elapsedTime = sf::Time::Zero;
}

void Animation::resume_playback() {
	is_playing = true;
}

sf::Time Animation::get_duration() const {
	sf::Time duration = sf::Time::Zero;
	for (const auto& i : vAnimFrames) {
		duration += i.dTime;
	}
	return duration;
}

TimeStruct Animation::get_time_info() const {
	if (vAnimFrames.empty()) return {};

	auto& f_time = vAnimFrames[0].dTime;
	sf::Time duration = f_time;
	sf::Time min = f_time;
	sf::Time max = f_time;

	for (size_t i = 1; i < vAnimFrames.size(); ++i) {
		auto& time = vAnimFrames[i].dTime;
		duration += time;
		if (min > time) {
			min = time;
		}
		if (max < time) {
			max = time;
		}
	}

	if (playbackSpeed != 1.f) duration = sf::seconds(duration.asSeconds() / playbackSpeed);
	return {duration, min, max};
}

float Animation::get_playback_speed() const {
	return playbackSpeed;
}

void Animation::DrawFramePreview() {
	auto& va = vAnimFrames[stCurrentObj].va;
	if (va.getPrimitiveType() != sf::Triangles) {
		DrawAnimPreview();
		ImGui::TextColored(DI::RED(), "Vertexes drawing not supported for this primitive type: ");
		ImGui::SameLine();
		DI::print_gui_primitive_type(va.getPrimitiveType());
		return;
	}

	static sf::RenderTexture rt_frame_preview;

	const float width = std::floor(ImGui::GetContentRegionAvail().x);
	const auto va_size_preview = sf::Vector2f(width, width);

	ImVec2 start_pos = ImGui::GetCursorScreenPos();
	ImGui::Dummy(ImVec2(width, width));

	sf::VertexArray va_triangle_1(sf::LineStrip, 4), va_triangle_2(sf::LineStrip, 4);
	for (size_t i = 0; i < 3; ++i) {
		va_triangle_1[i].position = va[i].position;
		va_triangle_1[i].color = sf::Color::Blue;

		va_triangle_2[i].position = va[i + 3].position;
		va_triangle_2[i].color = sf::Color::Red;
	}

	va_triangle_1[3].position = va[0].position;
	va_triangle_1[3].color = sf::Color::Blue;

	va_triangle_2[3].position = va[3].position;
	va_triangle_2[3].color = sf::Color::Red;

	ImGui::SetCursorScreenPos(start_pos);
	if (rt_frame_preview.getSize().x != (unsigned)va_size_preview.x || rt_frame_preview.getSize().y != (unsigned)va_size_preview.y) {
		rt_frame_preview.create((unsigned)va_size_preview.x, (unsigned)va_size_preview.y);
	}

	rt_frame_preview.clear(sf::Color::Transparent);

	sf::FloatRect b = va.getBounds();
	float padding = 2.f;
	sf::View v(sf::Vector2f(b.left + b.width/2.f, b.top + b.height/2.f), sf::Vector2f(b.width + padding, -b.height - padding));
	rt_frame_preview.setView(v);

	sf::RenderStates states_frame;
	states_frame.texture = vAnimFrames[stCurrentObj].texture;
	rt_frame_preview.draw(va, states_frame);

	rt_frame_preview.draw(va_triangle_1);
	rt_frame_preview.draw(va_triangle_2);
	rt_frame_preview.display();

	ImGui::Image(rt_frame_preview.getTexture());
}

bool Animation::on_frame_gui(AnimFrame& af) {
	ImGui::SeparatorText("Frame Inspector");

	ImGui::BulletText("Frame dTime:");
	float currentTime = af.dTime.asSeconds();
	if (ImGui::DragFloat("##dTime", &currentTime, 0.001f, 0.0f, 100.0f)) {
		af.dTime = sf::seconds(currentTime);
	}
	ImGui::Separator();

	ImGui::BulletText("Texture Info:");
	static bool is_preview = false;
	DI::show_texture_info(af.texture, is_preview, FLT_MAX);
	if (is_preview && ImGui::Button("Close Texture Preview")) {
		is_preview = false;
	} else if (!is_preview && ImGui::Button("Open Texture Preview")) {
		is_preview = true;
	}
	ImGui::Separator();

	ImGui::BulletText("VertexArray Info:");
	DI::show_va_info(af.va);
	ImGui::Separator();

	ImGui::BulletText("Frame preview:");
	//DrawAnimPreview();
	DrawFramePreview();
	ImGui::Separator();

	ImGui::BulletText("Vertexes:");
	const float min_height = 200.f;
	if (ImGui::BeginTable("VAChanger", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable
		| ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchSame, ImVec2(0, min_height))) {

		auto& va = af.va;

		ImGui::TableSetupColumn("Vertex", ImGuiTableColumnFlags_WidthFixed, 10.0f);
		ImGui::TableSetupColumn("Color");
		ImGui::TableSetupColumn("Position");
		ImGui::TableSetupColumn("TexCoords");

		ImGui::TableHeadersRow();

		for (size_t i = 0; i < va.getVertexCount(); ++i) {
			ImGui::TableNextRow();
			ImGui::PushID(i);

			auto& vertex = va[i];

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%lu", i);

			ImGui::TableSetColumnIndex(1);
			DI::show_color_info(vertex.color);

			ImGui::TableSetColumnIndex(2);
			auto pos = vertex.position;
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::DragFloat2("##vertex_position", pos)) {
				vertex.position = pos;
			}

			ImGui::TableSetColumnIndex(3);
			auto tex_coord = vertex.texCoords;
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::DragFloat2("##texCoords", tex_coord)) {
				vertex.texCoords = tex_coord;
			}

			ImGui::PopID();
		}

		ImGui::EndTable();
	}

	if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0))) {
		return true;
	}
	return false;
}

void Animation::DrawTimeline(TimeStruct& ts) {

	if (vAnimFrames.empty()) {
		ImGui::TextColored(DI::RED(), "No frames");
		return;
	}

	const float frame_width_min = 40.0f;
	const float frame_width_max = 200.0f;
	const float timeline_height = 80.0f;

	float total_width = 0.f;

	std::vector<float> rects_width;
	rects_width.reserve(vAnimFrames.size());

	const float dTimeAVG = ts.duration.asSeconds() / vAnimFrames.size();
	for (const auto& i : vAnimFrames) {
		auto& time = i.dTime;
		if (ts.dTimeMin >= time) {
			total_width += frame_width_min;
			rects_width.push_back(frame_width_min);
		} else {
			float diff = time.asSeconds() / dTimeAVG;
			float size = std::round(frame_width_min * diff) + frame_width_min;
			if (size > frame_width_max) size = frame_width_max;
			total_width += size;
			rects_width.push_back(size);
		}
	}

	ImGui::BulletText("Timeline:");
	if (ImGui::BeginChild("TimelineScroll", ImVec2(0, timeline_height), true, ImGuiWindowFlags_HorizontalScrollbar)) {
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 start_pos = ImGui::GetCursorScreenPos();

		ImGui::Dummy(ImVec2(total_width, timeline_height - 20));

		float curX = start_pos.x;
		float rect_height = 40.f;

		float line_pos = curX;

		for (int i = 0; i < (int)vAnimFrames.size(); ++i) {
			ImGui::PushID(i);

			ImVec2 b_min = ImVec2(curX, start_pos.y);
			ImVec2 b_max = ImVec2(curX + rects_width[i], b_min.y + rect_height);

			ImU32 color = (stCurrentObj == i) ? IM_COL32(100, 100, 255, 200) : IM_COL32(50, 50, 50, 200);
			draw_list->AddRectFilled(b_min, b_max, color);
			draw_list->AddRect(b_min, b_max, IM_COL32(200, 200, 200, 100));

			char buf[11]; snprintf(buf, sizeof(buf), "%d", i);
			draw_list->AddText(ImVec2(b_min.x + 5, b_min.y + 5), IM_COL32_WHITE, buf);

			ImGui::SetCursorScreenPos(b_min);
			if (ImGui::InvisibleButton("##f", ImVec2(rects_width[i], rect_height))) {
				stCurrentObj = i;
				pause_playback();
			}

			if (i == stCurrentObj) {
				line_pos = curX;
			}

			curX += rects_width[i];

			ImGui::PopID();
		}

		float playhead_diff = elapsedTime.asSeconds() / vAnimFrames[stCurrentObj].dTime.asSeconds();
		float playhead_frame_offset = rects_width[stCurrentObj] * playhead_diff;

		float playhead_x = line_pos + playhead_frame_offset;
		draw_list->AddLine(ImVec2(playhead_x, start_pos.y), ImVec2(playhead_x, start_pos.y + 60), IM_COL32(255, 0, 0, 255), 2.0f);

		ImGui::SameLine();
		if (is_playing && ImGui::Button("Stop")) {
			pause_playback();
		} else if (!is_playing && ImGui::Button("Play")) {
			resume_playback();
		}
	}
	ImGui::EndChild();
}

void Animation::DrawAnimPreview() const {
	static sf::RenderTexture rt_preview;
	const float width = std::floor(ImGui::GetContentRegionAvail().x);
	const auto va_size_preview = sf::Vector2f(width, width);
	DI::drawVA(vAnimFrames[stCurrentObj].va, va_size_preview, rt_preview, vAnimFrames[stCurrentObj].texture);
}

void Animation::on_gui() {
	if (ImGui::CollapsingHeader("AnimationSettings", ImGuiTreeNodeFlags_DefaultOpen)) {
		static sf::RenderTexture rt_tooltip;
		static std::vector<std::unique_ptr<sf::RenderTexture>> rt_vector;

		static int current_button_pressed = -1;

		TimeStruct ts(get_time_info());
		ImGui::BulletText("Info:");
		ImGui::Text("Duration: %.3fs", ts.duration.asSeconds());
		ImGui::Text("Min dTime: %.3fs", ts.dTimeMin.asSeconds());
		ImGui::Text("Max dTime: %.3fs", ts.dTimeMax.asSeconds());

		ImGui::BulletText("Playback speed:");
		ImGui::DragFloat("##playback_speed", &playbackSpeed, 0.001f, 0.001f, 100.f);

		ImGui::SeparatorText("Animation Frames");
		const auto va_size_frame = sf::Vector2f(100.f, 100.f);
		const auto va_size_tooltip = sf::Vector2f(300.f, 300.f);

		if (rt_vector.size() != vAnimFrames.size()) {
			rt_vector.resize(vAnimFrames.size());
		}

		float window_vis_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
		ImVec2 item_size(100, 135);
		ImGuiStyle& style = ImGui::GetStyle();

		for (size_t i = 0; i < vAnimFrames.size(); ++i) {
			ImGui::PushID(i);
			ImGui::BeginGroup();

			ImGui::Dummy(item_size);
			ImVec2 pos = ImGui::GetItemRectMin();
			ImVec2 pos_max = ImGui::GetItemRectMax();

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			ImU32 color = ImGui::GetColorU32(ImGuiCol_Border);

			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				if (current_button_pressed != i) {
					color = ImGui::GetColorU32(ImGuiCol_SeparatorActive);
					ImGui::TextDisabled("Click to inspect");
					ImGui::TextDisabled("Drag&Drop to swap frames");
				} else {
					ImGui::TextDisabled("Click to cancel inspecting");
				}
				DI::drawVA(vAnimFrames[i].va, va_size_tooltip, rt_tooltip, vAnimFrames[i].texture);
				ImGui::EndTooltip();
			}
			if (stCurrentObj == i) {
				color = IM_COL32(255, 0, 0, 255);
			}
			if (current_button_pressed == i) {
				color = IM_COL32(0, 255, 0, 255);
			}

			draw_list->AddRect(pos, pos_max, color, 4.0f);

			ImGui::SetCursorScreenPos(pos);

			ImGui::Text(" Frame: %lu", i);
			if (!rt_vector[i]) rt_vector[i] = std::make_unique<sf::RenderTexture>();
			DI::drawVA(vAnimFrames[i].va, va_size_frame, *rt_vector[i], vAnimFrames[i].texture);

			if (current_button_pressed != i) {
				ImGui::Text(" dTime: %.3fs", vAnimFrames[i].dTime.asSeconds());

				ImGui::SetCursorScreenPos(pos);
				if (ImGui::InvisibleButton("##f", item_size)) {
					stCurrentObj = i;
					pause_playback();
					current_button_pressed = i;
				}
			} else {
				ImGui::TextColored(DI::GREEN(), " Inspecting...");

				ImGui::SetCursorScreenPos(pos);
				if (ImGui::InvisibleButton("##f", item_size)) {
					stCurrentObj = i;
					resume_playback();
					current_button_pressed = -1;
				}
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("FRAME_INDEX", &i, sizeof(i));
				ImGui::Text("Moving frame %lu", i);
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FRAME_INDEX")) {
					size_t index_from = *(const size_t*)payload->Data;
					std::swap(vAnimFrames[i], vAnimFrames[index_from]);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::EndGroup();

			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + item_size.x;
			if (i + 1 < vAnimFrames.size() && next_button_x2 < window_vis_x2)
				ImGui::SameLine();

			ImGui::PopID();
		}

		if (current_button_pressed == -1) {
			ImGui::SeparatorText("Animation Preview");
			DrawAnimPreview();
			DrawTimeline(ts);
		} else {
			if (on_frame_gui(vAnimFrames[current_button_pressed])) {
				current_button_pressed = -1;
				resume_playback();
			}
		}
	}

	onGui();
}

Animation::~Animation() {
	if (is_updating) AnimationUpdate::delete_animation(obj_index);
}
