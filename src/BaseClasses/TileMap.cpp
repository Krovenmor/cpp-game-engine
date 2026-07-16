#include "BaseClasses/TileMap.h"
#include "EngineFiles/DrawableStorage.h"
#include "Utils/Logger.h"

#include "Utils/UsefulFunctions.h"

static Logger lTM("TileMap.log");

#define TILEMAP_TRANSFORMABLE_DATA_NAME "tilemaps_transformations_data"

void TileMap::on_gui() {
	// --- TileSet Settings ---
	auto ts_ptr = static_cast<TileSetContainer*>(this);
	ts_ptr->on_gui_set();
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (va != nullptr && tileset != nullptr) {
		states.transform *= getTransform();
		states.texture = tileset;
		target.draw(*va, states);
	} else {
		if (do_once) {
			lTM.error() << "class TileMap, virtual void draw(), Vertex array is null!, TileMapName: " << TileMapName << std::endl;
			do_once = false;
		}
	}
}

TileMap::TileMap(const char* TileMapName) : TransformableSaved(TileMapName, TILEMAP_TRANSFORMABLE_DATA_NAME, this),
TileMapName(TileMapName),
TileSetContainer((get_info() == "empty info") ? TileMapName : get_info()) {
	tilesetLoaded = load();
	if (tilesetLoaded) {
		TileSetName = getTilesetName();
		if (get_info() == "empty info") {
			save_info(TileMapName);
		}
	}
	DrawableStorage::check_in_tile(this);
}

TileMap::TileMap(const std::string& TileMapName) : TransformableSaved(TileMapName.c_str(), TILEMAP_TRANSFORMABLE_DATA_NAME, this),
TileMapName(TileMapName),
TileSetContainer((get_info() == "empty info") ? TileMapName : get_info()) {
	tilesetLoaded = load();
	if (tilesetLoaded) {
		TileSetName = getTilesetName();
		if (get_info() == "empty info") {
			save_info(TileMapName);
		}
	}
	DrawableStorage::check_in_tile(this);
}

TileMap::~TileMap() {
	DrawableStorage::check_out_tile(this);
}