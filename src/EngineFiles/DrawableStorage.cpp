#include <memory>
#include <SFML/Graphics/RenderWindow.hpp>

#include "EngineFiles/DrawableStorage.h"
#include "Utils/FastVector.hpp"

#include "BaseClasses/Actor.h"

#include "Utils/UsefulFunctions.h"
#include "Utils/Logger.h"

static Logger lDS("DrawableStorage.log");

namespace DrawableStorage {

	FastVector<Actor*> actors_high_priority;
	FastVector<Actor*> actors_med_priority;
	FastVector<Actor*> actors_low_priority;
	bool is_changed = false;

	std::vector<Actor*> ui_storage;
	std::vector<TileMap*> tilemap_storage;

	size_t check_in(Actor* actor_ptr, priority prior) {
		size_t index = 0;
		switch (prior) {
			case priority::HIGH_PRIOR:
				index = actors_high_priority.push(actor_ptr);
				break;
			case priority::MED_PRIOR:
				index = actors_med_priority.push(actor_ptr);
				break;
			case priority::LOW_PRIOR:
				index = actors_low_priority.push(actor_ptr);
				break;
		}
		is_changed = true;
		return index;
	}

	void check_out(const size_t& index, priority prior) {
		switch (prior) {
			case priority::HIGH_PRIOR:
				actors_high_priority.erase(index);
				break;
			case priority::MED_PRIOR:
				actors_med_priority.erase(index);
				break;
			case priority::LOW_PRIOR:
				actors_low_priority.erase(index);
				break;
		}
		is_changed = true;
	}

	void draw_actors(sf::RenderWindow& window, sf::Time elapsed) {
		for (auto& i : actors_low_priority) {
			i->on_tick(elapsed);
			window.draw(*i);
		}
		for (auto& i : actors_med_priority) {
			i->on_tick(elapsed);
			window.draw(*i);
		}
		for (auto& i : actors_high_priority) {
			i->on_tick(elapsed);
			window.draw(*i);
		}
	}

	void check_in_tile(TileMap* tilemap) {
		tilemap_storage.push_back(tilemap);
	}

	void check_out_tile(TileMap* tilemap) {
		if (tilemap) {
			auto it_where = std::find_if(tilemap_storage.begin(), tilemap_storage.end(),
				[tilemap](const auto& p) {
					return (p == tilemap);
				});

			if (it_where != tilemap_storage.end()) {
				fast_remove(tilemap_storage, it_where);
			}
			else {
				lDS.error() << "False check_out(TileMap* tilemap) in DrawableStorage!!!\n";
			}
		}
		else {
			lDS.error() << "Passed TileMap* is nullptr!!! check_out(TileMap* tilemap)\n";
		}
	}

	void check_in_ui(Actor* actor) {
		ui_storage.push_back(actor);
	}

	void check_out_ui(Actor* actor) {
		if (actor) {
			auto it_where = std::find_if(ui_storage.begin(), ui_storage.end(),
				[actor](const auto& p) {
					return (p == actor);
				});

			if (it_where != ui_storage.end()) {
				fast_remove(ui_storage, it_where);
			}
			else {
				lDS.error() << "False check_out_ui() in DrawableStorage!!!\n";
			}
		}
		else {
			lDS.error() << "Passed Actor* is nullptr!!! check_out_ui()\n";
		}
	}

	std::vector<Actor*>::const_iterator get_citer_end_ui() {
		return ui_storage.cend();
	}

	std::vector<Actor*>::const_iterator get_citer_begin_ui() {
		return ui_storage.cbegin();
	}

	std::vector<TileMap*>::const_iterator get_citer_begin_tile() {
		return tilemap_storage.cbegin();
	}

	std::vector<TileMap*>::const_iterator get_citer_end_tile() {
		return tilemap_storage.cend();
	}

	const std::vector<Actor*>& get_actors_storage() {
		static std::vector<Actor*> all_actors;
		if (is_changed) {
			all_actors.clear();
			auto all_size = actors_high_priority.size() + actors_med_priority.size() + actors_low_priority.size();
			all_actors.reserve(all_size);
			for (auto& i : actors_high_priority) {
				all_actors.push_back(i);
			}
			for (auto& i : actors_med_priority) {
				all_actors.push_back(i);
			}
			for (auto& i : actors_low_priority) {
				all_actors.push_back(i);
			}
			is_changed = false;
		}
		return all_actors;
	}

	const std::vector<Actor*>& get_UIactors_storage() {
		return ui_storage;
	}
	const std::vector<TileMap*>& get_tilemaps_storage() {
		return tilemap_storage;
	}
}
