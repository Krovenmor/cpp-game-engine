#include "EngineFiles/TileSetContainer.h"
#include "EngineFiles/Saving/JSONhandler.h"
#include "EngineFiles/Saving/SavingStructs/TileSet_SS.hpp"
#include "EngineFiles/FilesHandler.hpp"
#include "EngineFiles/ObjectStorage.hpp"

#include "UI/DefaultInterfaceTemplates.hpp"

#include "Utils/UsefulFunctions.h"

#include "imgui.h"


std::map<std::string, std::string> TileSetContainer::tilesets;
std::unordered_map<std::string, TileSetStruct> TileSetContainer::structures;

static Logger lTSC("TileSetContainer.log");


TileSetStruct& TileSetContainer::getFromStructures(const std::string& tileset_name) {
	auto it = structures.find(tileset_name);
	if (it == structures.end()) {
		structures[tileset_name] = jsonhandler::get<TileSetStruct>(tilesets[tileset_name]);
	}
	return structures[tileset_name];
}

bool TileSetContainer::addToStructures(const std::string& tileset_name, TileSetStruct struct_) {
	auto it = tilesets.find(tileset_name);
	if (it == tilesets.end()) {
		structures[tileset_name] = std::move(struct_);
		tilesets[tileset_name] = TILESETS_DATA_PATH + tileset_name + ".json";
		return save_all(tileset_name);
	}
	return false;
}

bool TileSetContainer::changeStructure(const std::string& tileset_name, TileSetStruct& struct_) {
	auto it_st = structures.find(tileset_name);
	auto it_ts = tilesets.find(tileset_name);
	if (it_st != structures.end() && it_ts != tilesets.end()) {
		structures[tileset_name] = std::move(struct_);
		return save_data(tileset_name);
	}
	return false;
}

bool TileSetContainer::changeName(const std::string& tileset_name, const std::string& new_tileset_name) {
	auto it_st = structures.find(tileset_name);
	auto it_ts = tilesets.find(tileset_name);
	if (it_st != structures.end() && it_ts != tilesets.end()) {
		if (file::delete_file(tilesets[tileset_name])) {
			structures[new_tileset_name] = std::move(structures[tileset_name]);
			tilesets[new_tileset_name] = TILESETS_DATA_PATH + new_tileset_name + ".json";
			structures.erase(tileset_name);
			tilesets.erase(tileset_name);
			return save_all(new_tileset_name);
		}
	}
	return false;
}

void TileSetContainer::init()
{
	if (tilesets.empty()) {
		auto config = jsonhandler::get<TileSetConfig>(CONFIG_TILESETS_PATH);
		if (jsonhandler::error) {
			jsonhandler::save<TileSetConfig>(CONFIG_TILESETS_PATH, TileSetConfig());
			lTSC.start_entry() << "No tilesets_config found in static void TileSetContainer::init()"
				<< ", so default config will be created at: " << CONFIG_TILESETS_PATH << std::endl;
		} else {
			tilesets = std::move(config.tilesets);
			if (tilesets.empty()) {
				lTSC.start_entry() << "No tilesets found in static void TileSetContainer::init()"
						<< ", so use add_new() func to add some...";
			}
		}
	}
}

bool TileSetContainer::add(const char *tileset_name, const std::string &tilesetPath, sf::Vector2u &tileSize,
const std::vector<std::vector<unsigned>> &tilesMappingContainer)
{
	bool result = addToStructures(tileset_name, {tilesetPath, tileSize, tilesMappingContainer});
	if (!result) {
		lTSC.error() << "Already existing tileset_name in static void add()" << std::endl;
	}
	return result;
}

bool TileSetContainer::add(const char* tilesetName, TileSetStruct& struct_)
{
	bool result = addToStructures(tilesetName, struct_);
	if (!result) {
		lTSC.error() << "Already existing tileset_name in static void add()" << std::endl;
	}
	return result;
}

bool TileSetContainer::save_config() {
	return jsonhandler::save<TileSetConfig>(CONFIG_TILESETS_PATH, {tilesets});
}

bool TileSetContainer::save_data(const std::string& data_name) {
	return jsonhandler::save<TileSetStruct>(tilesets[data_name], structures[data_name]);
}

bool TileSetContainer::save_all(const std::string& data_name) {
	return (save_config() && save_data(data_name));
}

std::pair<size_t, size_t> TileSetContainer::MakeCoordinates(int i, const size_t &allTileWidthx, const size_t &singleTileSizex)
{
	size_t coordinateY = 0, coordinateX = 0;
	int tiletest = i;
	if (tiletest > (allTileWidthx / singleTileSizex)) {
		while (tiletest > (allTileWidthx / singleTileSizex)) {
			tiletest -= static_cast<int>(allTileWidthx / singleTileSizex);
			coordinateY += 1;
		}
		coordinateX = tiletest - 1;
	}
	else {
		coordinateX = i - 1;
	}
	return { coordinateX, coordinateY };
}

bool check_TileSetStruct(const TileSetStruct& structure) {
    if (structure.texture_path.empty()) {
        lTSC.error() << "check_TileSetStruct(): Texture path is empty!" << std::endl;
        return false;
    }

    auto tileset = ObjectStorage::LoadTexture(structure.texture_path);
    if (!tileset) {
        lTSC.error() << "check_TileSetStruct(): Could not load texture: " << structure.texture_path << std::endl;
        return false;
    }

    sf::Vector2u texSize = tileset->getSize();

    if (structure.tileSize.x == 0 || structure.tileSize.y == 0) {
        lTSC.error() << "check_TileSetStruct(): TileSize components must be > 0" << std::endl;
        return false;
    }

    if (structure.tileSize.x > texSize.x || structure.tileSize.y > texSize.y) {
        lTSC.error() << "check_TileSetStruct(): TileSize (" << structure.tileSize.x << "x" << structure.tileSize.y
                     << ") exceeds texture dimensions (" << texSize.x << "x" << texSize.y << ")" << std::endl;
        return false;
    }

    unsigned tiles_per_row = texSize.x / structure.tileSize.x;
    unsigned tiles_per_col = texSize.y / structure.tileSize.y;
    unsigned max_allowed_index = (tiles_per_row * tiles_per_col) + 1;

    if (structure.tilesMappingContainer.empty()) {
        lTSC.error() << "check_TileSetStruct(): tilesMappingContainer is empty (0 rows)" << std::endl;
        return false;
    }

    size_t rows = structure.tilesMappingContainer.size();
    size_t cols = structure.tilesMappingContainer[0].size();

    if (cols == 0) {
        lTSC.error() << "check_TileSetStruct(): First row in container is empty (0 columns)" << std::endl;
        return false;
    }

    for (size_t r = 0; r < rows; ++r) {
        if (structure.tilesMappingContainer[r].size() != cols) {
            lTSC.error() << "check_TileSetStruct(): Inconsistent row size at index " << r
                         << " (expected " << cols << ", got " << structure.tilesMappingContainer[r].size() << ")" << std::endl;
            return false;
        }

        for (size_t c = 0; c < cols; ++c) {
            unsigned tile_idx = structure.tilesMappingContainer[r][c];

            if (tile_idx >= max_allowed_index) {
                lTSC.error() << "check_TileSetStruct(): Tile index [" << tile_idx << "] at grid (" << r << "," << c
                             << ") is out of range for texture (max index: " << max_allowed_index - 1 << ")" << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool TileSetContainer::load_existing(sf::VertexArray& va_, sf::Texture** tileset_, TileSetStruct &structure) {
	if (!check_TileSetStruct(structure)) return false;

	*tileset_ = ObjectStorage::LoadTexture(structure.texture_path);
	auto tileset = *tileset_;

	size_t multiplier = 6;

	unsigned height = structure.tilesMappingContainer.size();
	unsigned width = structure.tilesMappingContainer[0].size();

	sf::Vector2u size_of_texture = tileset->getSize();

	if (va_.getVertexCount() > 0) {
		va_.clear();
	}

	va_.setPrimitiveType(sf::PrimitiveType::Triangles);
	va_.resize(multiplier * width * height);

	size_t number = 0;
	for (unsigned y = 0; y != height; ++y) {
		for (unsigned x = 0; x != width; ++x) {

			size_t tile = structure.tilesMappingContainer[y][x];

			unsigned coordinateY = 0, coordinateX = 0, tileX = structure.tileSize.x, tileY = structure.tileSize.y;

			std::tie(coordinateX, coordinateY) = MakeCoordinates(static_cast<int>(tile), size_of_texture.x, tileX);

			coordinateY *= tileY;
			coordinateX *= tileX;

			size_t start = number * multiplier;

			//First
			va_[start].position = sf::Vector2f(x * tileX, y * tileY);
			va_[start + 1].position = sf::Vector2f(x * tileX + tileX, y * tileY);
			va_[start + 2].position = sf::Vector2f(x * tileX, y * tileY + tileY);
			//Second
			va_[start + 3].position = sf::Vector2f(x * tileX, y * tileY + tileY);
			va_[start + 4].position = sf::Vector2f(x * tileX + tileX, y * tileY + tileY);
			va_[start + 5].position = sf::Vector2f(x * tileX + tileX, y * tileY);

			//First
			va_[start].texCoords = sf::Vector2f(coordinateX, coordinateY);
			va_[start + 1].texCoords = sf::Vector2f(coordinateX + tileX, coordinateY);
			va_[start + 2].texCoords = sf::Vector2f(coordinateX, coordinateY + tileY);
			//Second
			va_[start + 3].texCoords = sf::Vector2f(coordinateX, coordinateY + tileY);
			va_[start + 4].texCoords = sf::Vector2f(coordinateX + tileX, coordinateY + tileY);
			va_[start + 5].texCoords = sf::Vector2f(coordinateX + tileX, coordinateY);

			number++;
		}
	}
	return true;
}

TileSetContainer::TileSetContainer(const char *tilesetName) : tileset_name(tilesetName) {
	init();
}

TileSetContainer::TileSetContainer(const std::string &tilesetName) : tileset_name(tilesetName) {
	init();
}

bool TileSetContainer::is_tilesetExists(const std::string& tilesetName) {
	if (safe_search(tilesets, tilesetName)) return true;
	return false;
}

bool TileSetContainer::add_new_tileSet(const char* tilesetName, const char *tilesetPath, sf::Vector2u tileSize,
                                       const std::vector<std::vector<unsigned>> &tilesMappingContainer) {
	TileSetStruct tileset_ = {tilesetPath, tileSize, tilesMappingContainer};
	return add_new_tileSet(tilesetName, tileset_);
}

bool TileSetContainer::add_new_tileSet(const char* tilesetName, TileSetStruct& ts_struct) {
	bool returnValue = false;
	if (check_TileSetStruct(ts_struct)) {
		if (add(tilesetName, ts_struct)) {
			returnValue = true;
		} else {
			lTSC.error() << "in add_new(), trouble with adding tileset: " << tilesetName;
		}
	} else {
		lTSC.error() << "in add_new(), wrong TileSetStruct: " << tilesetName;
	}
	return returnValue;
}

bool TileSetContainer::add_new_tileSet(const char *tilesetName, const char *tilesetPath,
	DefaultTileSet defaultTileSet) {
	TileSetStruct ts_struct;
	ts_struct.texture_path = tilesetPath;
	auto texture = ObjectStorage::LoadTexture(tilesetPath);
	if (texture && defaultTileSet == DefaultTileSet::OneTileHorizontal) {
		sf::Vector2u text_size = texture->getSize();
		sf::Vector2u tileSize = {text_size.y, text_size.y};
		size_t mapping_size = text_size.x / text_size.y;
		std::vector<std::vector<unsigned>> tilesMappingContainer;
		tilesMappingContainer.resize(1);
		tilesMappingContainer[0].resize(mapping_size);
		for (unsigned val = 1; val - 1 < mapping_size; val++) {
			tilesMappingContainer[0][val - 1] = val;
		}
		ts_struct.tileSize = tileSize;
		ts_struct.tilesMappingContainer = tilesMappingContainer;
		return add_new_tileSet(tilesetName, ts_struct);
	}
	lTSC.error() << "add_new_tileSet(DefaultTileSet): wrong tilesetPath: " << tilesetPath
		<< ", tilesetName: " << tilesetName;
	return false;
}

bool TileSetContainer::load()
{
	bool returnValue = false;
	if (!is_loaded) {
		if (is_tilesetExists(tileset_name)) {
			TileSetStruct& tileset_ref = getFromStructures(tileset_name);
			if (load_existing(va, &tileset, tileset_ref)) {
				is_loaded = true;
				returnValue = true;
				on_ptrs_changed();
				on_load();
			} else {
				lTSC.error() << "in bool load(), load_existing failed: " << tileset_name;
			}
		} else {
			lTSC.error() << "in bool load(), this TileSet wasn't founded in tilesets_config.json: "
				<< tileset_name;
		}
	} else {
		lTSC.error() << "in bool load(), this tileset already loaded: " << tileset_name;
	}
	return returnValue;
}

bool TileSetContainer::reload(const std::string& tilesetName) {
	unload();
	tileset_name = tilesetName;
	return load();
}

void TileSetContainer::unload() {
	tileset_name = "null";
	is_loaded = false;
	tileset = nullptr;
	va.clear();
}

sf::VertexArray TileSetContainer::make_tile_FromTileSet(size_t tile_number, sf::VertexArray* va_)
{
	size_t multiplier = 6;
	size_t start = tile_number * multiplier;
	sf::VertexArray va_return;
	if (start + multiplier > va_->getVertexCount())
	{
		lTSC.error() << "make_tile_FromTileSet: start + multiplier > vertex_count" << std::endl;
		return va_return;
	}

	va_return.setPrimitiveType(sf::PrimitiveType::Triangles);
	va_return.resize(multiplier);

	for (size_t i = 0; i < multiplier; i++, start++)
	{
		va_return[i].position = (*va_)[i].position;
		va_return[i].texCoords = (*va_)[start].texCoords;
	}
	return va_return;
}

sf::VertexArray TileSetContainer::make_single_tile_preview(unsigned tile_id, sf::Vector2u& tileSize, sf::Vector2u texSize) {
	sf::VertexArray tile_va(sf::Triangles, 6);
	if (tile_id == 0) return tile_va;

	unsigned tileX = tileSize.x;
	unsigned tileY = tileSize.y;

	unsigned coordX = 0, coordY = 0;
	std::tie(coordX, coordY) = MakeCoordinates(static_cast<int>(tile_id), texSize.x, tileX);

	coordX *= tileX;
	coordY *= tileY;

	tile_va[0].position = { 0.f, 0.f };
	tile_va[1].position = { (float)tileX, 0.f };
	tile_va[2].position = { 0.f, (float)tileY };
	tile_va[3].position = { 0.f, (float)tileY };
	tile_va[4].position = { (float)tileX, (float)tileY };
	tile_va[5].position = { (float)tileX, 0.f };

	tile_va[0].texCoords = { (float)coordX, (float)coordY };
	tile_va[1].texCoords = { (float)coordX + tileX, (float)coordY };
	tile_va[2].texCoords = { (float)coordX, (float)coordY + tileY };
	tile_va[3].texCoords = { (float)coordX, (float)coordY + tileY };
	tile_va[4].texCoords = { (float)coordX + tileX, (float)coordY + tileY };
	tile_va[5].texCoords = { (float)coordX + tileX, (float)coordY };

	return tile_va;
}

//
// GUI stuff
//

void TileSetContainer::on_change_tileSize(TileSetStruct& ts_struct, const sf::Texture* tileset_) {
	if (ts_struct.tileSize.x > 0 && ts_struct.tileSize.y > 0) {
		if (ts_struct.tileSize.x > tileset_->getSize().x) {
			ts_struct.tileSize.x = tileset_->getSize().x;
		}
		if (ts_struct.tileSize.y > tileset_->getSize().y) {
			ts_struct.tileSize.y = tileset_->getSize().y;
		}
		unsigned max_tile = (tileset_->getSize().x / ts_struct.tileSize.x) *
			 (tileset_->getSize().y / ts_struct.tileSize.y);
		for (auto& i : ts_struct.tilesMappingContainer) {
			for (auto& j : i) {
				if (j > max_tile) {
					j = max_tile;
				}
			}
		}
	}
}

bool TileSetContainer::deal_with_tilesMapping(TileSetStruct& ts_struct, bool is_init, sf::Texture* tileset_) {

	static int input_rows = 0;
	static int input_cols = 0;
	static unsigned default_input_id;
	static sf::RenderTexture rt_tooltip;
	static std::vector<std::vector<std::unique_ptr<sf::RenderTexture>>> rt_vector;
	static sf::Vector2i selected(-1, -1);

	bool is_changed = false;
	if (ImGui::CollapsingHeader("TilesMapping Settings")) {
		auto& vector = ts_struct.tilesMappingContainer;
		unsigned max_tile = (tileset_->getSize().x / ts_struct.tileSize.x) *
						 (tileset_->getSize().y / ts_struct.tileSize.y);

		unsigned step = 1;
		unsigned step_fast = 10;

		int current_rows = vector.size();
		int current_cols = current_rows > 0 ? (int)vector[0].size() : 0;

		if (is_init) {
			input_rows = current_rows;
			input_cols = current_cols;
			default_input_id = 1;
		}

		ImGui::Separator();
		if (ImGui::InputInt("Rows (Y)", &input_rows)) {
			input_rows = std::max(1, input_rows);
			vector.resize(input_rows);
			vector[input_rows - 1].resize(input_cols, default_input_id);
			current_rows = input_rows;
			is_changed = true;
		}
		if (ImGui::InputInt("Cols (X)", &input_cols)) {
			input_cols = std::max(1, input_cols);
			for (auto& row : vector) {
				row.resize(input_cols, default_input_id);
			}
			current_cols = input_cols;
			is_changed = true;
		}
		if (ImGui::InputScalar("Default Tile ID", ImGuiDataType_U32, &default_input_id, &step, &step_fast, "%u")) {
			default_input_id = std::min(default_input_id, max_tile);
		}

		ImGui::Separator();
		ImGui::BulletText("TileMapping info:");
		ImGui::Text("Min Tile id: %u", 1);
		ImGui::Text("Max Tile id: %u", max_tile);

		ImGui::Separator();
		if (current_rows > 0 && current_cols > 0) {
			const auto va_size_small = sf::Vector2f(100.f, 100.f);
			const auto va_size_big = sf::Vector2f(200.f, 200.f);

			float window_vis_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			ImVec2 item_size(100, 125);
			ImVec2 selectable_size = item_size;
			selectable_size.y -= 25;
			ImGuiStyle& style = ImGui::GetStyle();

			if (rt_vector.size() != vector.size()) {
				rt_vector.resize(vector.size());
			}

			if (rt_vector[0].size() != vector[0].size()) {
				const auto size = vector[0].size();
				for (auto& i : rt_vector) {
					i.resize(size);
				}
			}

			for (size_t i = 0; i < vector.size(); ++i) {
				auto tiles_length = vector[i].size();
				for (size_t j = 0; j < tiles_length; ++j) {
					int id = i * tiles_length + j;
					ImGui::PushID(id);
					ImGui::BeginGroup();

					auto& tile = vector[i][j];

					bool is_selected = (selected.x == i && selected.y == j);
					if (ImGui::Selectable("##item", is_selected, 0, selectable_size)) {
						if (selected.x != i && selected.y != j) {
							selected.x = i;
							selected.y = j;
						} else {
							selected = sf::Vector2i(-1, -1);
						}
					}
					ImVec2 pos = ImGui::GetItemRectMin();

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						ImGui::SetDragDropPayload("CONTAINER_TILE", &id, sizeof(id));
						ImGui::Text("Moving tile (index): %d", id);
						ImGui::EndDragDropSource();
					}
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTAINER_TILE")) {
							int index = *(const int*)payload->Data;
							int i_ = index / tiles_length;
							int j_ = index % tiles_length;
							std::swap(vector[i_][j_], tile);
							is_changed = true;

							if (selected.x == i_ && selected.y == j_) {
								selected.x = i;
								selected.y = j;
							}
						}
						ImGui::EndDragDropTarget();
					}

					auto to_draw = make_single_tile_preview(tile, ts_struct.tileSize, tileset_->getSize());
					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						if (selected.x == i && selected.y == j) {
							ImGui::TextDisabled("Click to cancel changing tile");
						} else {
							ImGui::TextDisabled("Click to change tile");
						}
						ImGui::TextDisabled("Drag&Drop to swap tiles");
						DI::drawVA(to_draw, va_size_big, rt_tooltip, tileset_);
						ImGui::EndTooltip();
					}

					ImGui::SetCursorScreenPos(pos);

					auto& rt = rt_vector[i][j];
					if (!rt) rt = std::make_unique<sf::RenderTexture>();
					DI::drawVA(to_draw, va_size_small, *rt, tileset_);

					if (is_selected) {
						ImGui::SetNextItemWidth(item_size.x);
						if (ImGui::InputScalar("##CurTile", ImGuiDataType_U32, &tile, NULL, NULL, "%u")) {
							if (tile > max_tile) {
								tile = max_tile;
							}
							is_changed = true;
						}
					} else {
						ImGui::Text("CurTile: %d", tile);
					}

					ImGui::EndGroup();

					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + item_size.x;
					if (j + 1 < tiles_length && next_button_x2 < window_vis_x2)
						ImGui::SameLine();

					ImGui::PopID();
				}
			}
		}
		ImGui::Separator();
	}
	return is_changed;
}

sf::Vector2f TileSetContainer::get_optimal_size_vaPreview(const sf::VertexArray& va_) {
	auto bounds = va_.getBounds();
	float width = ImGui::GetContentRegionAvail().x;
	float height = std::min(bounds.height * (width / bounds.width), ImGui::GetContentRegionAvail().x);
	return {width, height};
}

void TileSetContainer::draw_preview(const sf::VertexArray& va_, sf::Texture* tileset_) {
	static sf::RenderTexture render_Preview;

	ImGui::Separator();
	ImGui::Text("TileSet preview:");
	if (va_.getVertexCount() > 0 && tileset_) {
		DI::drawVA(va_, get_optimal_size_vaPreview(va_), render_Preview, tileset_);
	} else {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No texture loaded, and Vertex array not built yet!");
	}
	ImGui::Separator();
}

void TileSetContainer::tiles_size_settings_gui(TileSetStruct& ts_struct, sf::Texture* tileset_, sf::VertexArray& va_) {
	if (ImGui::CollapsingHeader("TilesSize Settings")) {
		unsigned texX = 0, texY = 0;
		if (tileset_) {
			auto size = tileset_->getSize();
			texX = size.x;
			texY = size.y;
		}

		ImGui::BulletText("TileSize (Width/Height):");
		auto tb = DI::DefaultTable2Columns("TileSize");

		tb.new_row("MinTileSize:");
		ImGui::Text("x: 1, y: 1");

		tb.new_row("MaxTileSize:");
		ImGui::Text("x: %u, y: %u", texX, texY);

		tb.new_row("TileSize x:");
		int size[2] = { (int)ts_struct.tileSize.x, (int)ts_struct.tileSize.y };
		ImGui::DragInt("##size_x", &size[0], 1, 1, (int)texX);

		tb.new_row("TileSize y:");
		ImGui::DragInt("##size_y", &size[1], 1, 1, (int)texY);

		if (size[0] != ts_struct.tileSize.x || size[1] != ts_struct.tileSize.y) {
			ts_struct.tileSize.x = size[0];
			ts_struct.tileSize.y = size[1];
			on_change_tileSize(ts_struct, tileset_);
			load_existing(va_, &tileset_, ts_struct);
		}
	}
}

bool TileSetContainer::on_gui_create() {

	static bool init = true;
	static bool v_mapping_init = true;
	static bool is_name_ok = true;
	static char input_buff[128] = "";
	static std::string name;
	static TileSetStruct ts_struct;
	static sf::Texture* tileset_;
	static sf::VertexArray va_;
	static sf::RenderTexture render_Preview;

	if (init) {
		ts_struct = TileSetStruct();
		ts_struct.tileSize = sf::Vector2u(1.f, 1.f);
		ts_struct.tilesMappingContainer = {{1}};
		ts_struct.texture_path = "empty";
		name = "New tileset";
		strncpy(input_buff, name.c_str(), sizeof(input_buff));
		tileset_ = nullptr;
		va_.clear();
		v_mapping_init = true;
		is_name_ok = true;

		init = false;
	}

	auto b = DI::Begin("TileSet Creator");

	ImGui::Text("Creating: %s", name.c_str());
	draw_preview(va_, tileset_);

	if (ImGui::CollapsingHeader("Main Settings")) {
		ImGui::BulletText("TileSetName");
		if (ImGui::InputText("<- Type new name", input_buff, sizeof(input_buff))) {
			name = input_buff;
			is_name_ok = true;
		}
		if (name.empty()) {
			ImGui::TextColored(DI::RED(), "Name can't be void");
			is_name_ok = false;
		}
		if (is_tilesetExists(name)) {
			ImGui::TextColored(DI::RED(), "This tileset name already exists");
			is_name_ok = false;
		}
		ImGui::Separator();

		if (sf::Texture* texture_return = DI::deal_with_texture(tileset_, ts_struct.texture_path)) {
			tileset_ = texture_return;
			load_existing(va_, &tileset_, ts_struct);
		}
	}

	tiles_size_settings_gui(ts_struct, tileset_, va_);
	if (deal_with_tilesMapping(ts_struct, v_mapping_init, tileset_)) {
		load_existing(va_, &tileset_, ts_struct);
		v_mapping_init = false;
	}

	if (ImGui::Button("Create")) {
		if (is_name_ok) {
			if (add_new_tileSet(name.c_str(), ts_struct)) {
				lTSC.start_entry() << "TileSet with name: '" << name << "' was created!";
				init = true;
				return true;
			}
		} else {
			lTSC.error() << "Name not ok!";
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		init = true;
		return true;
	}
	return false;
}

bool TileSetContainer::on_gui_change() {

	static bool init = true;
	static bool v_mapping_init = true;
	static bool is_name_ok = true;
	static char input_buff[128] = "";
	static std::string name;
	static TileSetStruct ts_struct;
	static sf::Texture* tileset_;
	static sf::VertexArray va_;

	if (init) {
		ts_struct = getFromStructures(tileset_name);
		name = tileset_name;
		strncpy(input_buff, name.c_str(), sizeof(input_buff));
		tileset_ = tileset;
		va_ = va;
		v_mapping_init = true;
		is_name_ok = true;

		init = false;
	}

	auto b = DI::Begin("TileSet Editor");

	ImGui::Text("Editing: %s", name.c_str());
	draw_preview(va_, tileset_);

	if (ImGui::CollapsingHeader("Main Settings")) {
		ImGui::BulletText("TileSetName");
		if (ImGui::InputText("<- Type new name", input_buff, sizeof(input_buff))) {
			name = input_buff;
			is_name_ok = true;
		}
		if (name.empty()) {
			ImGui::TextColored(DI::RED(), "Name can't be void");
			is_name_ok = false;
		}
		if (name != tileset_name && is_tilesetExists(name)) {
			ImGui::TextColored(DI::RED(), "This tileset name already exists");
			is_name_ok = false;
		}
		if (is_name_ok && tileset_name != name && ImGui::Button("Save Name")) {
			if (changeName(tileset_name, name)) {
				tileset_name = name;
				on_tileSetName_changed(tileset_name);
			}
		}
		ImGui::Separator();

		if (sf::Texture* texture_return = DI::deal_with_texture(tileset_, ts_struct.texture_path, FLT_MAX)) {
			tileset_ = texture_return;
			load_existing(va_, &tileset_, ts_struct);
		}
	}

	tiles_size_settings_gui(ts_struct, tileset_, va_);
	if (deal_with_tilesMapping(ts_struct, v_mapping_init, tileset_)) {
		load_existing(va_, &tileset_, ts_struct);
		v_mapping_init = false;
	}

	init = false;
	if (ImGui::Button("Save Changes")) {
		if (load_existing(va_, &tileset_, ts_struct)) {
			if (changeStructure(tileset_name, ts_struct)) {
				tileset = tileset_;
				va = va_;
				init = true;
				on_ptrs_changed();
				return false;
			}
			lTSC.error() << "Can't change struct";
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		init = true;
		return true;
	}
	return false;
}

bool is_change = false;
void TileSetContainer::on_gui_load_or_create() {
	ImGui::SeparatorText("Tileset Configuration");
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "[X] Choose some TileSet to load, or create new!");
	ImGui::Checkbox("Check to Create", &is_change);
	if (!is_change) {
		if (DI::defaultMapSelector(tilesets, "All available TileSets:", "TileSetName", "Path")) {
			if (reload(DI::map_selected_name)) {
				on_tileSetName_changed(DI::map_selected_name);
				DI::map_selected_name = "";
			} else {
				lTSC.error() << "Trouble with reloading, TileSetName: " << DI::map_selected_name << std::endl;
			}
		}
	} else {
		if (on_gui_create()) {
			is_change = false;
		}
	}
}

void TileSetContainer::on_gui_set() {
	if (!is_loaded) {
		on_gui_load_or_create();
	} else if (!is_change) {
		static sf::RenderTexture rt;

		if (ImGui::CollapsingHeader("TileSet Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
			TileSetStruct& tileset_ref = getFromStructures(tileset_name);
			ImGui::SeparatorText("Tileset Details");
			ImGui::Text("Name: %s", tileset_name.c_str());
			DI::print_path(30, "TileSet data path: %s", tilesets[tileset_name]);
			DI::print_path(30, "Texture path: %s", tileset_ref.texture_path);
			ImGui::Text("TileSize: %u x %u", tileset_ref.tileSize.x, tileset_ref.tileSize.y);
			ImGui::Spacing();

			ImGui::BulletText("TileSet preview:");
			DI::drawVA(va, get_optimal_size_vaPreview(va), rt, tileset);
			ImGui::Spacing();

			if (ImGui::Button("Change this Tileset")) {
				is_change = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Load or create another Tileset")) {
				unload();
			}
		}
	} else {
		if (on_gui_change()) {
			is_change = false;
		}
	}
}