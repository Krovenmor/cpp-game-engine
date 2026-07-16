#pragma once
#include <vector>

class Actor;
class TileMap;
enum class priority;

namespace sf
{
	class RenderWindow;
}

namespace DrawableStorage
{
	size_t check_in(Actor* actor_ptr, priority prior);
	void check_out(const size_t& index, priority prior);

	void draw_actors(sf::RenderWindow& window, sf::Time elapsed);

	void check_in_ui(Actor* actor);
	void check_out_ui(Actor* actor);

	void check_in_tile(TileMap* tilemap);
	void check_out_tile(TileMap* tilemap);

	std::vector<Actor*>::const_iterator get_citer_begin_ui();
	std::vector<Actor*>::const_iterator get_citer_end_ui();

	std::vector<TileMap*>::const_iterator get_citer_begin_tile();
	std::vector<TileMap*>::const_iterator get_citer_end_tile();

	const std::vector<Actor*>& get_actors_storage();
	const std::vector<Actor*>& get_UIactors_storage();
	const std::vector<TileMap*>& get_tilemaps_storage();
}
