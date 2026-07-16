#pragma once
#include <string>

namespace sf
{
	class Texture;
	class Font;
}

namespace ObjectStorage
{
	sf::Texture* LoadTexture(const std::string& filePath);
	sf::Font* LoadFont(const std::string& filePath);

	void ClearObjects();
}