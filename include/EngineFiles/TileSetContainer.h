#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>


enum class DefaultTileSet {
	OneTileHorizontal
};

struct TileSetStruct;

class TileSetContainer {
private:

	static std::map<std::string, std::string> tilesets;
	static std::unordered_map<std::string, TileSetStruct> structures;

	std::string tileset_name;
	bool is_loaded = false;
	sf::Texture* tileset = nullptr;
	sf::VertexArray va;

	static TileSetStruct& getFromStructures(const std::string& tileset_name);
	static bool addToStructures(const std::string& tileset_name, TileSetStruct struct_);
	static bool changeStructure(const std::string& tileset_name, TileSetStruct& struct_);
	static bool changeName(const std::string& tileset_name, const std::string& new_tileset_name);

	static void init();

	static bool add(const char* tileset_name, const std::string& tilesetPath, sf::Vector2u& tileSize,
	                const std::vector<std::vector<unsigned>>& tilesMappingContainer);

	static bool add(const char *tilesetName, TileSetStruct &struct_);

	static bool save_config();
	static bool save_data(const std::string &data_name);
	static bool save_all(const std::string &data_name);

	static std::pair<size_t, size_t> MakeCoordinates(int i, const size_t& allTileWidthx, const size_t& singleTileSizex);

	static bool load_existing(sf::VertexArray& va_, sf::Texture** tileset_, TileSetStruct &structure);

	// GUI //
	static sf::Vector2f get_optimal_size_vaPreview(const sf::VertexArray& va_);
	static void draw_preview(const sf::VertexArray& va_, sf::Texture* tileset_);
	static void on_change_tileSize(TileSetStruct& ts_struct, const sf::Texture* tileset_);
	static bool deal_with_tilesMapping(TileSetStruct& ts_struct, bool is_init, sf::Texture* tileset_);
	static void tiles_size_settings_gui(TileSetStruct& ts_struct, sf::Texture* tileset_, sf::VertexArray& va_);
	// END //

protected:

	virtual void on_ptrs_changed() {}
	virtual void on_tileSetName_changed(const std::string& new_tilesetName) {}
	virtual void on_load() {}

public:

	explicit TileSetContainer(const char* tilesetName);
	explicit TileSetContainer(const std::string& tilesetName);
	TileSetContainer(TileSetContainer& other) = delete;
	TileSetContainer(TileSetContainer&& other) = default;

	static bool is_tilesetExists(const std::string& tilesetName);
	bool is_tileset_loaded() const { return is_loaded; }

	static bool add_new_tileSet(const char* tilesetName, const char* tilesetPath, sf::Vector2u tileSize,
					const std::vector<std::vector<unsigned>>& tilesMappingContainer);
	static bool add_new_tileSet(const char* tilesetName, TileSetStruct &ts_struct);
	static bool add_new_tileSet(const char* tilesetName, const char* tilesetPath, DefaultTileSet defaultTileSet);

	size_t getFramesCount() { return va.getVertexCount() / 6;}
	sf::Texture* getTexture() { return tileset; }
	sf::VertexArray* getVertexArray() { return &va; }
	const std::string& getTilesetName() const { return tileset_name; }

	bool load();
	bool reload(const std::string& tilesetName);
	void unload();

	sf::VertexArray make_tile_FromTileSet(size_t tile_number, sf::VertexArray* va_);
	static sf::VertexArray make_single_tile_preview(unsigned tile_id, sf::Vector2u& tileSize, sf::Vector2u texSize);

	// returns true when done
	static bool on_gui_create();

	// returns true when done
	bool on_gui_change();
	void on_gui_load_or_create();
	void on_gui_set();

	virtual ~TileSetContainer() = default;
};
