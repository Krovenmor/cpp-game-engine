#include "EngineFiles/FilesHandler.hpp"

#include <iostream>

#include "imgui.h"
#include "imfilebrowser.h"
#include "Utils/Logger.h"

namespace file
{
	Logger lFH("FilesHandler.log");

	ImGui::FileBrowser fileDialog;

	void init(const char* label, const std::vector<std::string>&& types_) {
		fileDialog.SetTitle(label);
		fileDialog.SetTypeFilters(types_);
		fileDialog.SetDirectory(std::filesystem::current_path());
	}

	std::string to_relative(const std::filesystem::path& path) {
		static std::string current_dir = std::filesystem::current_path().filename().string() + "/";
		std::string full_path = path.string();
		size_t pos_start = full_path.find(current_dir);
		size_t pos_end = pos_start + current_dir.size();
		return full_path.substr(pos_end, full_path.size() - pos_end);
	}

	bool b_init = true;
	std::string selected_file;
	bool select_file(SelectType type) {
		if (b_init) {
			switch (type) {
				case SelectType::Texture:
					init("Textures", {".png", ".jpg", ".jpeg", ".bmp", ".tga"});
					break;
				case SelectType::Font:
					init("Fonts", {".ttf", ".otf"});
					break;
			}
			fileDialog.Open();
			b_init = false;
		}

		fileDialog.Display();

		if (!fileDialog.IsOpened()) {
			selected_file.clear();
			b_init = true;
			return true;
		}
		if (fileDialog.HasSelected()) {
			auto path = fileDialog.GetSelected();
			selected_file = to_relative(path);
			fileDialog.ClearSelected();
			b_init = true;
			return true;
		}
		return false;
	}

	std::string get_selected_file() {
		std::string temp(std::move(selected_file));
		selected_file.clear();
		return temp;
	}

	bool delete_file(const std::string& filePath) {
		std::filesystem::path file_path = filePath;
		try {
			if (!std::filesystem::remove(file_path)) {
				lFH.error() << "Trouble removing file: " << filePath << std::endl;
			}
		} catch (const std::filesystem::filesystem_error& e) {
			lFH.error()  << "Exception on removing file: " << e.what() << ", file: " << filePath << std::endl;
		}
		return true;
	}
}
