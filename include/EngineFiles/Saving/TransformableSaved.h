#pragma once
#include <string>
#include <SFML/Graphics/Transformable.hpp>

#include "UI/ImGuiHelpers.h"
#include "SavingComponent.hpp"
#include "EngineFiles/Saving/SavingStructs/Transformable_SS.hpp"

inline Logger lTS("TransformableSaved.log");


template <class T>
class TransformableSaved : public sf::Transformable {
private:

	inline static std::map<std::string, unsigned> id_s;
	SavingComponent<TransformableConfig, T> s;

	std::string name;
	T* t_ptr;

	void apply_changes() {
		auto& conf = s.get_config();
		auto it = conf.um_structs.find(name);
		if (it != conf.um_structs.end()) {
			auto& u_map = conf.um_structs[name];
			t_ptr->setPosition(u_map.position);
			t_ptr->setRotation(u_map.rotation);
			t_ptr->setScale(u_map.scale);
			t_ptr->setOrigin(u_map.origin);
		}
	}

protected:

	virtual void on_gui() {};

public:

	TransformableSaved(const char* name, const char* config_name_, T* t_ptr) : s((TRANSFORMABLE_DATA_PATH + std::string(config_name_) + ".json").c_str()) {
		const unsigned id = id_s[name];
		this->name = name + std::string(" (id: ") + std::to_string(id) + ")";
		id_s[name] = id + 1;
		this->t_ptr = t_ptr;
		apply_changes();
	}

	TransformableSaved(const std::string& name, const char* config_name_, T* t_ptr) : s((TRANSFORMABLE_DATA_PATH + std::string(config_name_) + ".json").c_str()) {
		const unsigned id = id_s[name];
		this->name = name + " (id: " + std::to_string(id) + ")";
		id_s[name] = id + 1;
		this->t_ptr = t_ptr;
		apply_changes();
	}

	void save(const std::string& Name) {
		auto& conf = s.get_config();
		auto& u_map = conf.um_structs[Name];
		u_map.origin = t_ptr->getOrigin();
		u_map.scale = t_ptr->getScale();
		u_map.rotation = t_ptr->getRotation();
		u_map.position = t_ptr->getPosition();
		s.save_config();
	}

	bool delete_record() {
		auto& conf = s.get_config();
		auto& u_map = conf.um_structs;
		auto it = u_map.find(name);
		if (it != u_map.end()) {
			u_map.erase(it);
			s.save_config();
			return true;
		}
		return false;
	}

	void save_info(const std::string& info) {
		auto& conf = s.get_config();
		conf.um_structs[name].info = info;
		s.save_config();
	}

	const std::string& get_info() const {
		static std::string empty_info = "empty info";
		const auto& conf = s.get_config();
		auto it = conf.um_structs.find(name);
		if (it != conf.um_structs.end()) {
			return it->second.info;
		}
		return empty_info;
	}

	void OnGui() {
		static std::string inspector_title = " Inspector";

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
		ImGui::TextUnformatted((name + inspector_title).c_str());
		ImGui::PopStyleColor();

		ImGui::Separator();
		// --- Transforming ---
		if (ImGui::CollapsingHeader("Transforming Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
			auto pos = t_ptr->getPosition();
			if (ImGui::DragFloat2("Position", pos)) {
				t_ptr->setPosition(pos);
			}
			float rotation = t_ptr->getRotation();
			if (ImGui::DragFloat("Rotation", &rotation)) {
				t_ptr->setRotation(rotation);
			}
			auto scale = t_ptr->getScale();
			if (ImGui::DragFloat2("Scale", scale, 0.01f)) {
				t_ptr->setScale(scale);
			}
			auto origin = t_ptr->getOrigin();
			if (ImGui::DragFloat2("Origin", origin)) {
				t_ptr->setOrigin(origin);
			}

			ImGui::Separator();
			if (ImGui::Button("Save Changes", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				save(name);
			}
		}
		on_gui();
	}
};