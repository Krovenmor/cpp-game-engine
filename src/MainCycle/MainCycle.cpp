#include <thread>
#include "MainCycle/MainCycle.h"

#include "BaseClasses/TileMap.h"
#include "BaseClasses/WidgetActor.h"

#include "EngineFiles/Saving/SavingStructs/Engine_SS.hpp"
#include "EngineFiles/DrawableStorage.h"
#include "EngineFiles/MapHandler.hpp"
#include "EngineFiles/Animation/AnimationUpdate.hpp"
#include "EngineFiles/CollisionManager.hpp"
#include "EngineFiles/InputManager.hpp"
#include "EngineFiles/ObjectStorage.hpp"

#include "Utils/Logger.h"
#include "Utils/GameTools.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

#include "UI/ImGuiHandler.h"


//Global
static bool bIsMouseOnWindow = false;
static Logger lMain("MainCycle.log");

int MainCycle(game& currentGame) {
	{
		auto config = jsonhandler::get<GameEngineConfig>(CONFIG_SETTINGS_PATH);
		if (jsonhandler::error) {
			lMain.start_entry() << "Looks like you don't have setting_config.json in assets/config/"
			<< "\nSo it will be created with default values" << std::endl;
			if (!jsonhandler::save<GameEngineConfig>(CONFIG_SETTINGS_PATH, config)) {
				lMain.error() << "Create assets/config/ dir!" << std::endl;
			}
		}

		sf::ContextSettings settings;
		settings.antialiasingLevel = config.antialiasingLevel;
		sf::RenderWindow window(sf::VideoMode(config.resX, config.resY), config.window_title, sf::Style::Default, settings);
		window.setVerticalSyncEnabled(config.is_vertical_sync_enabled);
		window.setFramerateLimit(config.framerate_limit);

		if (!ImGui::SFML::Init(window)) {
			lMain.error() << "Could not init ImGui!" << std::endl;
			return -1;
		}

		sf::Clock clock;
		sf::Time elapsed;

		// All for UI
		bool is_ui_resize = false;
		float width = window.getSize().x, height = window.getSize().y;
		sf::View ui_view(sf::FloatRect(0.f, 0.f, width, height));

		// For stopping game_tick, when window is not focused
		bool bIsFocused = true;

		MapHandler::init();
		currentGame.init(window);
		ImGuiHandler::initUI();
		lMain.start_entry("All ok, starting game loop!");
		while (window.isOpen()) {

			sf::Event event;
			while (window.pollEvent(event)) {
				ImGui::SFML::ProcessEvent(window, event);
				switch (event.type) {
				case sf::Event::Closed: {
					window.close();
					break;
				}
				case sf::Event::Resized: {
					ui_view.reset(sf::FloatRect(0.f, 0.f, window.getSize().x, window.getSize().y));
					is_ui_resize = true;
					currentGame.on_event(event, window);
					break;
				}
				case sf::Event::GainedFocus: {
					bIsFocused = true;
					break;
				}
				case sf::Event::LostFocus: {
					bIsFocused = false;
					break;
				}
				case sf::Event::MouseEntered: {
					bIsMouseOnWindow = true;
					break;
				}
				case sf::Event::MouseLeft: {
					bIsMouseOnWindow = false;
					break;
				}
				case sf::Event::KeyPressed:
					[[fallthrough]];
				case sf::Event::KeyReleased:
					InputManager::on_event(event);
					[[fallthrough]];
				default:
					currentGame.on_event(event, window);
					break;
				}
			}
			//
			//

			//
			// Needs to create MovementComponent ( or Pawn, Character class )
			//
			if (bIsFocused) {

				data::elapsedTime = elapsed;

				// UI
				ImGui::SFML::Update(window, elapsed);
				ImGui::ShowDemoWindow();

				//Collision search
				CollisionManager::search();

				window.clear();

				currentGame.game_tick(elapsed, window);

				// First of all we need to draw tileMaps
				for (auto iter = DrawableStorage::get_citer_begin_tile(); iter != DrawableStorage::get_citer_end_tile(); ++iter) {
					window.draw(**iter);
				}

				// Apply animations
				AnimationUpdate::check_and_update(elapsed);

				//Draw
				//All actors
				DrawableStorage::draw_actors(window, elapsed);

				//Draw Widgets
				window.setView(ui_view);
				for (auto iter = DrawableStorage::get_citer_begin_ui(); iter != DrawableStorage::get_citer_end_ui(); ++iter) {
					if (is_ui_resize) {
						auto* w_ptr = static_cast<WidgetActor*>(*iter);
						w_ptr->on_resized(window.getSize().x, window.getSize().y, width, height);
					}
					window.draw(**iter);
				}
				if (is_ui_resize) {
					is_ui_resize = false;
					width = window.getSize().x, height = window.getSize().y;
				}

				// UI
				ImGuiHandler::handleUI();
				ImGui::SFML::Render(window);

				window.display();
				elapsed = clock.restart();

				//Deleting all spawnables
				spawn::check();
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
			}
		}

		ImGui::SFML::Shutdown();
		MapHandler::close();
		ImGuiHandler::saveUI();
		ObjectStorage::ClearObjects();
		lMain.start_entry("End game loop :|");
	}
	return 0;
}


