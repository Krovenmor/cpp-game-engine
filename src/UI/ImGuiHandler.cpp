#include "UI/ImGuiHandler.h"
#include "UI/DefaultInterfaceTemplates.hpp"
#include "UI/Output.hpp"

#include "BaseClasses/TileMap.h"
#include "BaseClasses/Actor.h"

#include "EngineFiles/DrawableStorage.h"
#include "EngineFiles/MapHandler.hpp"
#include "EngineFiles/SystemFactory.hpp"
#include "EngineFiles/Saving/SavingStructs/UI_SS.hpp"

#include "imgui.h"


namespace Redactor
{
	template <class Item>
	void ItemHandle(std::string& class_name, const std::vector<Item*>& item_storage) {
		static Item* selectedItem = nullptr;
		static size_t selectedItemIndex = 0;

		for (int i = 0; i < item_storage.size(); i++) {
			char label[128];
			std::snprintf(label, sizeof(label),"%s ##%d", item_storage[i]->get_name().c_str(), i);

			if (ImGui::Selectable(label, selectedItem == item_storage[i])) {
				selectedItem = item_storage[i];
				selectedItemIndex = i;
			}
		}

		if (selectedItem && selectedItemIndex < item_storage.size() && selectedItem == item_storage[selectedItemIndex]) {
			bool is_open = true;

			static std::string InspectorLabel = class_name + " Inspector";
			static std::string ButtonLabel = "Delete " + class_name;

			if (ImGui::Begin(InspectorLabel.c_str(), &is_open)) {
				if (MapHandler::is_item_MapHandled<Item>(selectedItem->get_name())) {
					ImGui::Separator();
					if (ImGui::Button(ButtonLabel.c_str())) {
						const std::string name = selectedItem->get_name();
						if (MapHandler::remove_item_from_map<Item>(name)) {
							selectedItem = nullptr;
						} else {
							std::cerr << "ItemHandle: trouble with deleting " << class_name << std::endl;
						}
					} else {
						ImGui::Separator();
						selectedItem->OnGui();
					}
				} else {
					selectedItem->OnGui();
				}
			}
			ImGui::End();
			if (!is_open) {
				selectedItem = nullptr;
			}

		} else if (selectedItem) {
			bool is_open = true;
			ImGui::Begin("Item was destroyed", &is_open);
			ImGui::TextColored(DI::RED(), "%s", (class_name + " was destroyed").c_str());
			if (!is_open || ImGui::Button("Close")) {
				selectedItem = nullptr;
			}
			ImGui::End();
		}
	}

	void redactor(bool& is_redactor) {
		if (is_redactor) {
			auto b = DI::Begin("Redactor", is_redactor);
			if (ImGui::CollapsingHeader("Actors")) {
				static std::string ClassNameActors = "Actor";
				ItemHandle<Actor>(ClassNameActors, DrawableStorage::get_actors_storage());
			}
			ImGui::Separator();
			if (ImGui::CollapsingHeader("TileMaps")) {
				static std::string ClassNameTileMaps = "TileMap";
				ItemHandle<TileMap>(ClassNameTileMaps, DrawableStorage::get_tilemaps_storage());
			}
		}
	}
}

namespace Creator
{
	bool is_error = false;
	bool is_create_map = false;
	bool is_create_tilemap = false;
	bool is_create_actor = false;
	bool is_create_tileset = false;

	void error() {
		if (is_error) {
			ImGui::Spacing();
			ImGui::TextColored(DI::RED(), "%s", (std::string("Error: ") + MapHandler::get_last_error()).c_str());
			ImGui::Spacing();
		}
	}

	void createMap() {

	}

	void createActor() {
		static std::string ActorName;
		static std::string ActorInstance;
		auto clean_resources = [&]() {
			ActorName.clear();
			ActorInstance.clear();
			is_create_actor = false;
			is_error = false;
		};

		auto b = DI::Begin("Create Actor");
		ImGui::SeparatorText("Actor Configuration");

		error();

		if (ActorInstance.empty()) {
			ImGui::BulletText("Choose Actor to create");
			if (ActorName.empty() && DI::defaultVectorSelector(ActorFactory::getRegisteredNames(), "All registered actors:", "Actor Classes")) {
				if (!DI::vector_selected_name.empty()) {
					ActorInstance = DI::vector_selected_name;
					DI::vector_selected_name = "";
				}
			}
			if (ImGui::Button("Exit")) {
				clean_resources();
			}
		} else {
			ImGui::BulletText("Selected Actor Class: %s", ActorInstance.c_str());
			ImGui::SameLine(0.0f, 20.0f);
			if (ImGui::SmallButton("Edit")) {
				ActorInstance.clear();
			}
			ImGui::Separator();

			ImGui::Text("Enter Actor name:");
			static char text[128];
			if (ImGui::InputText("##", text, sizeof(text))) {
				ActorName = text;
			}

			if (ImGui::Button("Create")) {
				if (MapHandler::add_item_to_map<Actor>(ActorName, ActorInstance)) {
					text[0] = '\0';
					clean_resources();
				} else {
					is_error = true;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Exit")) {
				text[0] = '\0';
				clean_resources();
			}
		}
	}

	void createTileMap() {
		auto b = DI::Begin("Create TileMap", is_create_tilemap);
		ImGui::BulletText("Enter new TileMap name:");

		error();

		static char text[128];
		static std::string new_map_name;
		if (ImGui::InputText("##", text, sizeof(text))) {
			new_map_name = text;
		}

		if (ImGui::Button("Create")) {
			if (MapHandler::add_item_to_map<TileMap>(new_map_name)) {
				text[0] = '\0';
				new_map_name.clear();
				is_create_tilemap = false;
				is_error = false;
			} else {
				is_error = true;
			}
		}
	}

	void createTileSet() {
		if (TileSetContainer::on_gui_create()) {
			is_create_tileset = false;
		}
	}

	void createMenu(bool& is_create_menu) {
		if (is_create_menu) {
			auto b = DI::Begin("Create menu", is_create_menu);

			if (DI::default_button_with_text("New Map",
				"Create new Map")) {
				is_create_menu = false;
				is_create_map = true;
			}

			if (DI::default_button_with_text("New TileMap",
				"Create new TileMap using already existing TileSet, or creating a new one")) {
				is_create_menu = false;
				is_create_tilemap = true;
			}

			if (DI::default_button_with_text("New Actor",
				"Create new base Actor class, or your registered derived from Actor class")) {
				is_create_menu = false;
				is_create_actor = true;
			}

			if (DI::default_button_with_text("New TileSet",
				"Create new TileSet")) {
				is_create_menu = false;
				is_create_tileset = true;
			}

		} else {
			if (is_create_tilemap) {
				createTileMap();
			}
			if (is_create_map) {
				createMap();
			}
			if (is_create_actor) {
				createActor();
			}
			if (is_create_tileset) {
				createTileSet();
			}
		}
	}
}

namespace ImGuiHandler
{
	bool is_create_menu = false;
	bool is_redactor = false;
	UIConfig conf;

	void handleMenu() {
		float alpha = (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive()) ? 1.0f : 0.0f;
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.1f, 0.1f, 0.1f, alpha));

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Open")) {
				if (ImGui::MenuItem("Create new")) { is_create_menu = true; }
				if (ImGui::MenuItem("Redactor")) { is_redactor = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::PopStyleColor();
	}

	void initUI() {
		conf = jsonhandler::get<UIConfig>(CONFIG_UI_PATH);
		if (jsonhandler::error) {
			jsonhandler::save<UIConfig>(CONFIG_UI_PATH, conf);
		}
		is_create_menu = conf.is_create_menu;
		is_redactor = conf.is_redactor;
		Output::set_is_collapsed(conf.is_output);
	}

	void saveUI() {
		conf.is_create_menu = is_create_menu;
		conf.is_redactor = is_redactor;
		conf.is_output = Output::get_is_collapsed();
		jsonhandler::save<UIConfig>(CONFIG_UI_PATH, conf);
	}

	void handleUI() {
		handleMenu();
		Output::draw();
		Creator::createMenu(is_create_menu);
		Redactor::redactor(is_redactor);
	}
}