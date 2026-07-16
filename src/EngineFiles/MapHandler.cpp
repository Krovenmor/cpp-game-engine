#include "EngineFiles/MapHandler.hpp"
#include "EngineFiles/Saving/SavingStructs/Map_SS.hpp"

#include "BaseClasses/TileMap.h"
#include "BaseClasses/Actor.h"
#include "EngineFiles/SystemFactory.hpp"

#include "Utils/UsefulFunctions.h"
#include "Utils/Logger.h"


#define DEFAULT_MAP "default_map"


namespace MapHandler
{
	Logger lMH("MapHandler.log");

	std::string last_error;

	std::string default_map;
	std::string current_map;
	std::map<std::string, std::string> maps;

	MapStruct current_map_data;
	std::unordered_map<std::string, std::unique_ptr<TileMap>> tilemaps;
	std::unordered_map<std::string, std::unique_ptr<Actor>> actors;

	auto& get_container(TileMap*) { return tilemaps; }
	auto& get_container(Actor*) { return actors; }
	auto& get_map_data_container(TileMap*) {return current_map_data.tileMaps;}
	auto& get_map_data_container(Actor*) {return current_map_data.Actors;}

	unsigned c_changes = 0;
	unsigned m_changes = 0;

	void save_map() {
		if (m_changes > 0) {
			jsonhandler::save<MapStruct>(maps[current_map], current_map_data);
			m_changes = 0;
		}
	}

	void save_config() {
		if (c_changes > 0) {
			jsonhandler::save<MapsConfig>(CONFIG_MAPS_PATH, MapsConfig(default_map, std::move(maps)));
			c_changes = 0;
		}
	}

	void destroy_map() {
		save_map();
		tilemaps.clear();
	}

	void spawn_all_entities(MapStruct& map_data) {
		tilemaps.reserve(map_data.tileMaps.size());
		actors.reserve(map_data.Actors.size());
		for (auto& name : map_data.tileMaps) {
			tilemaps[name] = std::make_unique<TileMap>(name);
		}
		auto& registry = ActorFactory::getRegistry();
		for (auto& [instance, name] : map_data.Actors) {
			auto it = registry.find(instance);
			if (it != registry.end()) {
				actors[name] = it->second(name.c_str());
			} else {
				lMH.error() << "spawn_all_entities(): Class: '" << instance << ", with name: " << name
				<< "' wasn't found in registry!";
			}
		}
	}

	bool open_map(const std::string& map_name) {
		if (maps.find(map_name) != maps.end()) {
			destroy_map();
			current_map = map_name;
			current_map_data = jsonhandler::get<MapStruct>(maps[map_name]);
			spawn_all_entities(current_map_data);
			return true;
		}
		last_error = "open_map(), map wasn't found: " + map_name;
		lMH << last_error;
		return false;
	}

	void first_create() {
		default_map = DEFAULT_MAP;
		maps[default_map] = MAPS_DATA_PATH + default_map + ".json";
		MapsConfig config_default;
		config_default.default_map = default_map;
		config_default.maps_paths = maps;
		jsonhandler::save<MapsConfig>(CONFIG_MAPS_PATH, config_default);
		jsonhandler::save<MapStruct>(maps[default_map], MapStruct(default_map));
	}

	void init() {
		auto config = jsonhandler::get<MapsConfig>(CONFIG_MAPS_PATH);
		maps = std::move(config.maps_paths);
		default_map = std::move(config.default_map);
		if (jsonhandler::error) {
			first_create();
		}
		open_map(default_map);
	}

	void close() {
		destroy_map();
		save_config();
	}

	bool new_map(const std::string& new_map_name) {
		if (!new_map_name.empty() && maps.find(new_map_name) == maps.end()) {
			maps[new_map_name] = MAPS_DATA_PATH + new_map_name + ".json";
			if (jsonhandler::save<MapStruct>(maps[new_map_name], MapStruct(new_map_name))) {
				c_changes++;
				return true;
			}
		} else {
			last_error = "Trouble with new_map()";
			lMH.error() << last_error;
		}
		return false;
	}

	template <class Item>
	bool add_item_to_map(const std::string& new_item_name, const std::string& instance) {
		if (!new_item_name.empty()) {
			auto ptr = (Item*)nullptr;
			auto& container = get_container(ptr);
			auto& map_data_container = get_map_data_container(ptr);
			if constexpr (std::is_base_of_v<Actor, Item>) {
				auto& registry = ActorFactory::getRegistry();
				auto it = registry.find(instance);
				if (it != registry.end()) {
					if (container.find(new_item_name) == container.end()) {
						container[new_item_name] = it->second(new_item_name.c_str());
						map_data_container.emplace_back(instance, new_item_name);
						m_changes++;
						return true;
					}
					last_error = "add_item_to_map(), this actor name already exists: " + new_item_name;
					lMH.error() << last_error;
				} else {
					last_error = "add_item_to_map(), this actor instance: '" + instance + "' wasn't found in registry!";
					lMH.error() << last_error;
				}
			} else {
				if (container.find(new_item_name) == container.end()) {
					container[new_item_name] = std::make_unique<Item>(new_item_name);
					map_data_container.emplace_back(new_item_name);
					m_changes++;
					return true;
				}
				last_error = "add_item_to_map(), this TileMap name already exists: " + new_item_name;
				lMH.error() << last_error;
			}
		} else {
			last_error = "Trouble with add_item_to_map(), new_item_name is empty !";
			lMH.error() << last_error;
		}
		return false;
	}

	template <class Item>
	bool remove_item_from_map(const std::string& item_name) {
		if (!item_name.empty()) {
			auto ptr = (Item*)nullptr;
			auto& container = get_container(ptr);
			auto& map_data_container = get_map_data_container(ptr);

			auto it_1 = container.find(item_name);
			if (it_1 != container.end()) {

				auto& vector = map_data_container;
				if constexpr (std::is_base_of_v<Actor, Item>) {
					auto it_2 = std::find_if(vector.begin(), vector.end(), [&item_name](auto& p) {
						return p.second == item_name;
					});
					if (it_2 != vector.end()) {
						it_1->second->on_delete();
						container.erase(it_1);
						fast_remove(vector, it_2);

						m_changes++;
						return true;
					}
				} else {
					auto it_2 = std::find(vector.begin(), vector.end(), item_name);
					if (it_2 != vector.end()) {
						it_1->second->delete_record();
						container.erase(it_1);
						fast_remove(vector, it_2);

						m_changes++;
						return true;
					}
				}
			}
		}
		last_error = "Trouble with remove_item_from_map(), item_name: " + item_name;
		lMH.error() << last_error;
		return false;
	}

	template <class Item>
	bool is_item_MapHandled(const std::string& item_name) {
		auto& container = get_container((Item*)nullptr);
		auto it_1 = container.find(item_name);
		if (it_1 != container.end()) {
			return true;
		}
		return false;
	}

	const std::string& get_last_error() {
		return last_error;
	}

	template bool add_item_to_map<TileMap>(const std::string&, const std::string&);
	template bool add_item_to_map<Actor>(const std::string&, const std::string&);

	template bool remove_item_from_map<TileMap>(const std::string&);
	template bool remove_item_from_map<Actor>(const std::string&);

	template bool is_item_MapHandled<TileMap>(const std::string&);
	template bool is_item_MapHandled<Actor>(const std::string&);
}
