#include "EngineFiles/ObjectStorage.hpp"

#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Utils/Logger.h"


namespace ObjectStorage
{
	Logger lOS("ObjectStorage.log");

	std::unordered_map<std::string, sf::Font> storage_font;
	std::unordered_map<std::string, sf::Texture> storage_texture;

	auto& get_storage(sf::Font*) {return storage_font;}
	auto& get_storage(sf::Texture*) {return storage_texture;}

	template <class A>
	A* LoadAndGetDrawableObj(const std::string& filePath) {
		A* r_val = nullptr;
		if (!filePath.empty()) {
			auto& storage = get_storage((A*)nullptr);

			auto it = storage.find(filePath);
			if (it == storage.end()) {
				A a;
				if (a.loadFromFile(filePath)) {
					auto [it_, inserted] = storage.insert({ filePath, a });
					r_val = &it_->second;
				} else {
					lOS.error() << "LoadAndGetDrawableObj, problem with loading from file, filepath: " << filePath;
				}
			} else {
				r_val = &(it->second);
			}
		} else {
			lOS.error() << "LoadAndGetDrawableObj, filepath is empty !";
		}
		return r_val;
	}

	sf::Texture* LoadTexture(const std::string& filePath) {
		return LoadAndGetDrawableObj<sf::Texture>(filePath);
	}
	sf::Font* LoadFont(const std::string& filePath) {
		return LoadAndGetDrawableObj<sf::Font>(filePath);
	}

	void ClearObjects() {
		storage_font.clear();
		storage_texture.clear();
	}

	template sf::Texture* LoadAndGetDrawableObj(const std::string& filePath);
	template sf::Font* LoadAndGetDrawableObj(const std::string& filePath);
}
