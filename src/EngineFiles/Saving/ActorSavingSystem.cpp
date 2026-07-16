#include "EngineFiles/Saving/ActorSavingSystem.hpp"

#include "Utils/UsefulFunctions.h"
#include "EngineFiles/FilesHandler.hpp"


std::unordered_set<std::string> ActorSavingClass::all_actors;

static Logger lASS("ActorSavingClass.log");


void ActorSavingClass::delete_data() {
	auto& conf = config.get_config();
	auto it_conf = conf.actors.find(actor_name);
	if (it_conf != conf.actors.end()) {
		if (!file::delete_file(it_conf->second)) {
			lASS.error() << "Trouble with deleting file, path: " << it_conf->second;
		}
		conf.actors.erase(it_conf);
		config.save_config();
	}
}

void ActorSavingClass::init_data() {
	auto& conf = config.get_config();
	auto it = conf.actors.find(actor_name);
	if (it == conf.actors.end()) {
		std::string path = TRANSFORMABLE_DATA_PATH + actor_name + "_data.json";
		if (jsonhandler::save<ActorData>(path, data)) {
			conf.actors[actor_name] = path;
			config.save_config();
		} else {
			lASS.error() << "init_data: Trouble with first saving data";
		}
	} else {
		data = jsonhandler::get<ActorData>(conf.actors[actor_name]);
		if (jsonhandler::error) {
			lASS.error() << "init_data: Trouble with getting data!";
		}
	}
}

void ActorSavingClass::save_data() const {
	std::string path = TRANSFORMABLE_DATA_PATH + actor_name + "_data.json";
	if (!jsonhandler::save<ActorData>(path, data)) {
		lASS.error() << "save_data: Trouble with saving data";
	}
}

ActorSavingClass::ActorSavingClass(const std::string& actor_name) :
actor_name(actor_name), config(CONFIG_ACTOR_PATH) {
	init_data();
	auto [it, bool_] = all_actors.insert(actor_name);
	if (!bool_) {
		lASS.error() << "Actors_name not unique!!!";
	}
}

void ActorSavingClass::init_sprite(const char* sprite_name, const int obj_id, const std::unique_ptr<sf::Sprite>& new_sprite) {
	auto& sprite_settings = data.sprites_settings;
	auto it = sprite_settings.find(sprite_name);
	if (it == sprite_settings.end()) {
		sprite_settings[sprite_name] = SpriteSettings();
		save_data();
	} else {
		it->second.apply(new_sprite.get());
	}
	actors_drawable[obj_id] = sprite_name;
}

void ActorSavingClass::init_text(const char* text_name, const int obj_id, const std::unique_ptr<sf::Text>& new_text) {
	auto& text_settings = data.texts_settings;
	auto it = text_settings.find(text_name);
	if (it == text_settings.end()) {
		text_settings[text_name] = TextSettings();
		save_data();
	} else {
		it->second.apply(new_text.get());
	}
	actors_drawable[obj_id] = text_name;
}

void ActorSavingClass::on_sprite_changed(const std::string& sprite_name, sf::Sprite* new_sprite, const std::string& new_text_path) {
	auto& sprite_settings = data.sprites_settings;
	auto it = sprite_settings.find(sprite_name);
	if (it != sprite_settings.end()) {
		if (new_sprite) {
			it->second.save(new_sprite);
			it->second.texture_path = new_text_path;
			save_data();
		} else {
			lASS.error() << "on_sprite_changed: new_sprite is nullptr!";
		}
	} else {
		lASS.error() << "on_sprite_changed: this sprite_name not found: " << sprite_name;
	}
}

void ActorSavingClass::on_text_changed(const std::string& text_name, sf::Text* new_text, const std::string& new_font_path) {
	auto& text_settings = data.texts_settings;
	auto it = text_settings.find(text_name);
	if (it != text_settings.end()) {
		if (new_text) {
			it->second.save(new_text);
			it->second.font_path = new_font_path;
			save_data();
		} else {
			lASS.error() << "on_text_changed: new_text is nullptr!";
		}
	} else {
		lASS.error() << "on_text_changed: this text_name not found: " << text_name;
	}
}

std::string ActorSavingClass::get_name(const int obj_id) const {
	std::string r_name;
	auto it = actors_drawable.find(obj_id);
	if (it != actors_drawable.end()) {
		r_name = it->second;
	}
	return r_name;
}
