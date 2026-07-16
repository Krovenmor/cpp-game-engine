#include "EngineFiles/Saving/SavingStructs/Actor_SS.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "EngineFiles/ObjectStorage.hpp"


void SpriteSettings::apply(sf::Sprite* sprite_ptr) {
	if (!texture_path.empty()) {
		auto texture = ObjectStorage::LoadTexture(texture_path);
		if (texture) {
			sprite_ptr->setTexture(*texture);
		}
	}
	sprite_ptr->setColor(color);
	sprite_ptr->setPosition(position);
	sprite_ptr->setScale(scale);
	sprite_ptr->setRotation(rotation);
	sprite_ptr->setOrigin(origin);
	sprite_ptr->setTextureRect(rect);
}

void SpriteSettings::save(sf::Sprite* sprite_ptr) {
	color = sprite_ptr->getColor();
	position = sprite_ptr->getPosition();
	scale = sprite_ptr->getScale();
	rotation = sprite_ptr->getRotation();
	origin = sprite_ptr->getOrigin();
	rect = sprite_ptr->getTextureRect();
}

void TextSettings::apply(sf::Text *text_ptr) {
	if  (!font_path.empty()) {
		auto font = ObjectStorage::LoadFont(font_path);
		if (font) {
			text_ptr->setFont(*font);
		}
	}
	text_ptr->setOutlineColor(outline_color);
	text_ptr->setFillColor(fill_color);
	text_ptr->setOutlineThickness(thickness);
	text_ptr->setCharacterSize(size);
	text_ptr->setLetterSpacing(letter_spacing);
	text_ptr->setLineSpacing(line_spacing);
	text_ptr->setPosition(position);
	text_ptr->setScale(scale);
	text_ptr->setRotation(rotation);
	text_ptr->setOrigin(origin);
	text_ptr->setString(text);
	text_ptr->setStyle(style);
}

void TextSettings::save(sf::Text *text_ptr) {
	outline_color = text_ptr->getOutlineColor();
	fill_color = text_ptr->getFillColor();
	thickness = text_ptr->getOutlineThickness();
	size = text_ptr->getCharacterSize();
	letter_spacing = text_ptr->getLetterSpacing();
	line_spacing = text_ptr->getLineSpacing();
	position = text_ptr->getPosition();
	scale = text_ptr->getScale();
	rotation = text_ptr->getRotation();
	origin = text_ptr->getOrigin();
	text = text_ptr->getString();
	style = text_ptr->getStyle();
}