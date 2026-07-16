#pragma once
#include <fstream>

#include "json.hpp"
#include "Utils/Logger.h"


#define CONFIG_SETTINGS_PATH "assets/config/settings_config.json"
#define CONFIG_MAPS_PATH "assets/config/maps_config.json"
#define CONFIG_TILESETS_PATH "assets/config/tilesets_config.json"
#define CONFIG_UI_PATH "assets/config/ui_config.json"

#define CONFIG_PATH "assets/config/"

#define ACTOR_CONFIG_NAME "actors_config"

#define TRANSFORMABLE_DATA_PATH "assets/data/"
#define TILESETS_DATA_PATH "assets/tilesets/"
#define MAPS_DATA_PATH "assets/maps/"


using json = nlohmann::json;
namespace jsonhandler {

    inline Logger lJSON("JSONhandler.log");
    inline bool error = false;

    template <class Class, class StringLike>
    Class get(const StringLike& path) {
        std::ifstream file(path);
        Class class_;
        if (file.is_open()) {
            try {
                json j;
                file >> j;
                class_ = j.get<Class>();
                lJSON.start_entry() << "Class successfully read get(), path: " << path << std::endl;
                error = false;
                return class_;
            }
            catch (const json::parse_error& e) {
                lJSON.error() << "\nTrouble with parsing get(): " << e.what() << std::endl;
            }
            catch (const json::type_error& e) {
                lJSON.error() << "\nType error in get(): " << e.what() << std::endl;
            }
            catch (const std::exception& e) {
                lJSON.error() << "\nError in get(): " << e.what() << std::endl;
            }
        }
        else {
            lJSON.error() << "\nCannot open file in get(): " << path << std::endl;
        }
        error = true;
        return class_;
    }

    template <class Class, class StringLike>
    bool save(const StringLike path, const Class& class_) {
        std::ofstream file(path);
        bool r_value = false;

        if (file.is_open()) {
            try {
                json j = class_;

                file << j.dump(4);

                lJSON.start_entry() << "Config successfully saved to: " << path;
                r_value = true;
            }
            catch (const std::exception& e) {
                lJSON.error() << "Error in save(): " << e.what();
            }
        }
        else {
            lJSON.error() << "Cannot open file for writing in save(): " << path;
        }
        return r_value;
    }
}
