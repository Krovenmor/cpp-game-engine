#pragma once
#include <unordered_set>

#include "SavingComponent.hpp"
#include "SavingStructs/Actor_SS.hpp"


class Actor;
struct ActorConfig;

class ActorSavingClass {
private:

	static std::unordered_set<std::string> all_actors;

	SavingComponent<ActorConfig, Actor> config;
	ActorData data;
	std::map<int, std::string> actors_drawable;

	std::string actor_name;

	void init_data();
	void save_data() const;

public:

	ActorSavingClass(const std::string& actor_name);

	void init_sprite(const char* sprite_name, int obj_id, const std::unique_ptr<sf::Sprite>& new_sprite);
	void init_text(const char *text_name, int obj_id, const std::unique_ptr<sf::Text> &new_text);

	void on_sprite_changed(const std::string& sprite_name, sf::Sprite* new_sprite, const std::string& new_text_path);
	void on_text_changed(const std::string &text_name, sf::Text *new_text, const std::string &new_font_path);

	std::string get_name(int obj_id) const;
	ActorData& get_data() { return data; }
	void delete_data();

};