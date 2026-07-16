#pragma once
#include "JSONhandler.h"

template <class Config, class T>
class SavingComponent {
private:

	inline static bool do_once = true;
	inline static Config config;
	inline static std::string config_path;

	static void init(const char* config_path_) {
		if (do_once) {
			config_path = config_path_;
			config = jsonhandler::get<Config>(config_path);
			if (jsonhandler::error) {
				config = Config();
				if (!jsonhandler::save<Config>(config_path, config)) {
					std::cerr << "Problem in TransformableSaved::init() with saving config";
				}
			}
			do_once = false;
		}
	}

public:

	SavingComponent(const char* config_path_) {
		init(config_path_);
	}

	Config& get_config() { return config; }
	const Config& get_config() const { return config; }

	bool save_config() {
		return jsonhandler::save<Config>(config_path, config);
	}

};