#pragma once
#include <SFML/Graphics.hpp>

#include "EngineFiles/TileSetContainer.h"
#include "EngineFiles/Saving/TransformableSaved.h"

class TileMap : public sf::Drawable, public TransformableSaved<TileMap>, public TileSetContainer {
private:

	sf::Texture* tileset = nullptr;
	sf::VertexArray* va = nullptr;
	std::string TileMapName;
	std::string TileSetName;
	bool tilesetLoaded = false;
	mutable bool do_once = true;

	void on_gui() override;
	void on_ptrs_changed() override { tileset = getTexture(); va = getVertexArray(); }
	void on_tileSetName_changed(const std::string& new_name) override { TileSetName = new_name; save_info(new_name); }
	void on_load() override { tilesetLoaded = true; }

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:

	explicit TileMap(const char* TileMapName);
	explicit TileMap(const std::string& TileMapName);

	const std::string& get_name() const { return TileMapName; }

	~TileMap() override;
};