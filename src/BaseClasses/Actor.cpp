#include "BaseClasses/Actor.h"

#include "Components/CollisionComponent.h"
#include "Components/InputComponent.h"
#include "Components/AnimationComponent.h"

#include "EngineFiles/Saving/ActorSavingSystem.hpp"
#include "EngineFiles/DrawableStorage.h"
#include "EngineFiles/Animation/Animation.hpp"

#include "UI/DefaultInterfaceTemplates.hpp"

#include "EngineFiles/ObjectStorage.hpp"
#include "Utils/GameTools.hpp"
#include "Utils/Logger.h"


#define ACTOR_TRANSFORMABLE_DATA_NAME "actors_transformations_data"
static Logger lActor("Actor.log");

template <class Item>
void Actor::on_gui_item(const DrawableObj& obj, const std::string& type, size_t obj_id) {
	static Item* inspected_item = nullptr;
	auto ptr = static_cast<Item*>(obj.to_draw.get());

	std::string item_name;
	if (actor_saving) {
		item_name = actor_saving->get_name(obj_id);
		std::string full = "Type: " + type + ", name %s, obj_id: %d";
		ImGui::BulletText(full.c_str(), item_name.c_str(), obj_id);
	} else {
		std::string full = "Type: " + type + ", obj_id: %d";
		ImGui::BulletText(full.c_str(), obj_id);
	}

	if (!inspected_item || (inspected_item != ptr)) {
		ImGui::SameLine();
		ImGui::PushID(obj_id);
		if (ImGui::SmallButton("Inspect")) {
			inspected_item = ptr;
		}
		ImGui::PopID();
		ImGui::SameLine();
		if (!item_name.empty()) {
			DI::TooltipWithColoredText(DI::GREEN(), "[V]", "Means it's actor item, and you actually save changes");
		} else {
			DI::TooltipWithColoredText(DI::RED(), "[X]", "Means it's not actor item, it won't be saved");
		}
	}

	if (inspected_item && inspected_item == ptr) {
		static std::string item_path;

		if constexpr (std::is_same_v<Item, sf::Sprite>) {

			if (item_path.empty() && !item_name.empty()) {
				item_path = actor_saving->get_data().sprites_settings[item_name].texture_path;
			}

			if (DI::deal_with_sprite(inspected_item, item_path)) {
				if (!item_name.empty() && DI::get_is_save_sprite()) {
					actor_saving->on_sprite_changed(item_name, inspected_item, item_path);
				}
				item_path.clear();
				inspected_item = nullptr;
			}

		} else if constexpr (std::is_same_v<Item, sf::Text>) {

			if (item_path.empty() && !item_name.empty()) {
				item_path = actor_saving->get_data().texts_settings[item_name].font_path;
			}

			if (DI::deal_with_text(inspected_item, item_path)) {
				if (!item_name.empty() && DI::get_is_save_text()) {
					actor_saving->on_text_changed(item_name, inspected_item, item_path);
				}
				item_path.clear();
				inspected_item = nullptr;
			}

		} else {
			lActor.error() << "Trouble with on_gui_item!";
		}
	}
}

void Actor::on_gui_vertex(const DrawableObj &obj, size_t obj_id) {
	static bool is_show = false;
	static int obj_show = -1;

	ImGui::BulletText("Type: sf::VertexArray, obj_id: %lu", obj_id);

	ImGui::PushID(obj_id);
	ImGui::SameLine();
	if (is_show && obj_show == obj_id) {
		if (ImGui::SmallButton("Close info")) {
			is_show = false;
			obj_show = -1;
		} else {
			auto ptr = static_cast<sf::VertexArray*>(obj.to_draw.get());
			DI::show_va_info(*ptr);
		}
	}
	else if (ImGui::SmallButton("Show info")) {
		is_show = true;
		obj_show = obj_id;
	}
	ImGui::PopID();
}

template <class Component>
void gui_component(Actor* this_ptr, const std::string& label) {
	auto component = dynamic_cast<Component*>(this_ptr);
	if (component) {
		static bool is_showing = false;
		ImGui::TextColored(DI::GREEN(), "%s", (label + " attached").c_str());
		ImGui::SameLine();
		if (!is_showing) {
			if (ImGui::SmallButton(("Show info##" + label).c_str())) {
				is_showing = true;
			}
		}
		if (is_showing) {
			if (ImGui::SmallButton(("Close info##" + label).c_str())) {
				is_showing = false;
			}
			component->on_gui();
		}
	} else {
		ImGui::TextColored(DI::RED(), "%s", (label + " not included").c_str());
	}
}

void Actor::on_gui_components() {
	ImGui::SeparatorText("Info on attached components");

	gui_component<CollisionComponent>(this, "CollisionComponent");
	gui_component<InputComponent>(this, "InputComponent");
	gui_component<AnimationComponent>(this, "AnimationComponent");
}

void Actor::on_gui() {

	on_gui_components();
	ImGui::SeparatorText("Actor Info");

	ImGui::Text("Actor drawing priority:");
	ImGui::SameLine();
	if (prior == priority::LOW_PRIOR) {
		ImGui::Text(" LOW");
	}
	if (prior == priority::MED_PRIOR) {
		ImGui::Text(" MED");
	}
	if (prior == priority::HIGH_PRIOR) {
		ImGui::Text(" HIGH");
	}

	ImGui::SeparatorText("All Drawable obj");
	if (!actor_drawable_storage.empty()) {
		for (size_t i = 0; i < actor_drawable_storage.size(); i++) {
			auto& obj = actor_drawable_storage[i];
			switch (obj.type) {
				case DrawableType::Sprite:
					static std::string sprite_type = "Sprite";
					on_gui_item<sf::Sprite>(obj, sprite_type, i);
					break;
				case DrawableType::Text:
					static std::string text_type = "Text";
					on_gui_item<sf::Text>(obj, text_type, i);
					break;
				case DrawableType::VertexArray:
					on_gui_vertex(obj, i);
					break;
				case DrawableType::Animation:
					ImGui::BulletText("Type: Animation, obj_id: %lu", i);
					break;
			}
		}
	} else {
		ImGui::Text("No drawable obj");
	}
}

void Actor::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = NULL;

	for (const auto& val : actor_drawable_storage) {
		if (val.is_visible) {
			if (val.to_draw) {
				target.draw(*val.to_draw, states);
			}
			else {
				lActor.error() << "nullptr in virtual void draw()";
			}
		}
	}

	on_draw(target, states);
}

Actor::Actor(const char* actor_name) : TransformableSaved(actor_name, ACTOR_TRANSFORMABLE_DATA_NAME, this) {
	name = actor_name;
	ActorIndex = DrawableStorage::check_in(this, prior);
}

Actor::Actor(const std::string& actor_name) : TransformableSaved(actor_name, ACTOR_TRANSFORMABLE_DATA_NAME, this) {
	name = actor_name;
	ActorIndex = DrawableStorage::check_in(this, prior);
}

Actor::Actor(const char* actor_name, priority prior) : TransformableSaved(actor_name, ACTOR_TRANSFORMABLE_DATA_NAME, this) {
	name = actor_name;
	this->prior = prior;
	ActorIndex = DrawableStorage::check_in(this, prior);
}

Actor::Actor(const std::string &actor_name, priority prior): TransformableSaved(actor_name, ACTOR_TRANSFORMABLE_DATA_NAME, this) {
	name = actor_name;
	this->prior = prior;
	ActorIndex = DrawableStorage::check_in(this, prior);
}

Actor::Actor(const char* ui_name, bool is_ui) : TransformableSaved(ui_name, ACTOR_TRANSFORMABLE_DATA_NAME, this), is_ui(true) {
	name = ui_name;
	DrawableStorage::check_in_ui(this);
}

void Actor::init_actor_saving() {
	if (!actor_saving) {
		actor_saving = new ActorSavingClass(name);
		if (!actor_saving) {
			lActor.error() << "init_actor_saving: trouble with init ptr!";
		}
	}
}

int Actor::get_id() const {
	if (actor_drawable_storage.empty()) return 0;
	return  actor_drawable_storage.size() - 1;
}

int Actor::AnnounceSprite(const char* sprite_name) {
	init_actor_saving();
	if (auto sprite_ptr = std::make_unique<sf::Sprite>()) {
		auto id = get_id();
		actor_saving->init_sprite(sprite_name, id, sprite_ptr);
		actor_drawable_storage.emplace_back(std::move(sprite_ptr), true, DrawableType::Sprite);
		return id;
	}
	return -1;
}

int Actor::AnnounceText(const char* text_name) {
	init_actor_saving();
	if (auto text_ptr = std::make_unique<sf::Text>()) {
		auto id = get_id();
		actor_saving->init_text(text_name, id, text_ptr);
		actor_drawable_storage.emplace_back(std::move(text_ptr), true, DrawableType::Text);
		return id;
	}
	return -1;
}

int Actor::AddSprite(sf::Sprite& sprite) {
	if (auto sprite_ptr = std::make_unique<sf::Sprite>()) {
		*sprite_ptr = sprite;
		actor_drawable_storage.emplace_back(std::move(sprite_ptr), true, DrawableType::Sprite);
		return actor_drawable_storage.size() - 1;
	}
	return -1;
}

int Actor::AddSprite(const std::string& texture_path) {
	if (auto texture = ObjectStorage::LoadTexture(texture_path)) {
		if (auto sprite_ptr = std::make_unique<sf::Sprite>()) {
			sprite_ptr->setTexture(*texture);
			actor_drawable_storage.emplace_back(std::move(sprite_ptr), true, DrawableType::Sprite);
			return actor_drawable_storage.size() - 1;
		}
		lActor.error() << "sprite_ptr is NULL in Actor::AddSprite() with path: " << texture_path;
	}
	else {
		lActor.error() << "texture is NULL in Actor::AddSprite() with path: " << texture_path;
	}
	return -1;
}

int Actor::AddText(sf::Text& text) {
	if (auto text_ptr = std::make_unique<sf::Text>(text)) {
		actor_drawable_storage.emplace_back(std::move(text_ptr), true, DrawableType::Text);
		return actor_drawable_storage.size() - 1;
	}
	return -1;
}

int Actor::AddText(const std::string& text, const std::string& font_path) {
	if (auto text_ptr = std::make_unique<sf::Text>()) {
		auto font = ObjectStorage::LoadFont(font_path);
		if (font) {
			text_ptr->setFont(*font);
			text_ptr->setString(text);
			actor_drawable_storage.emplace_back(std::move(text_ptr), true, DrawableType::Text);
			return actor_drawable_storage.size() - 1;
		}
	}
	return -1;
}

int Actor::AddText() {
	if (auto text_ptr = std::make_unique<sf::Text>()) {
		actor_drawable_storage.emplace_back(std::move(text_ptr), true, DrawableType::Text);
		return actor_drawable_storage.size() - 1;
	}
	return -1;
}

int Actor::AddVertexArray(sf::VertexArray& va) {
	if (auto va_ptr = std::make_unique<sf::VertexArray>(va)) {
		actor_drawable_storage.emplace_back(std::move(va_ptr), true, DrawableType::VertexArray);
		return actor_drawable_storage.size() - 1;
	}
	return -1;
}

sf::Sprite* Actor::get_sprite(int obj_id) {
	if (obj_id >= 0 && obj_id < actor_drawable_storage.size()) {
		DrawableObj& obj = actor_drawable_storage[obj_id];
		if (obj.type == DrawableType::Sprite) {
			return static_cast<sf::Sprite*>(obj.to_draw.get());
		}
	}
	return nullptr;
}

sf::Text* Actor::get_text(int obj_id) {
	if (obj_id >= 0 && obj_id < actor_drawable_storage.size()) {
		DrawableObj& obj = actor_drawable_storage[obj_id];
		if (obj.type == DrawableType::Text) {
			return static_cast<sf::Text*>(obj.to_draw.get());
		}
	}
	return nullptr;
}

sf::VertexArray* Actor::get_va(int obj_id) {
	if (obj_id >= 0 && obj_id < actor_drawable_storage.size()) {
		DrawableObj& obj = actor_drawable_storage[obj_id];
		if (obj.type == DrawableType::VertexArray) {
			return static_cast<sf::VertexArray*>(obj.to_draw.get());
		}
	}
	return nullptr;
}

Animation* Actor::get_anim(int obj_id) {
	if (obj_id >= 0 && obj_id < actor_drawable_storage.size()) {
		DrawableObj& obj = actor_drawable_storage[obj_id];
		if (obj.type == DrawableType::Animation) {
			return static_cast<Animation*>(obj.to_draw.get());
		}
	}
	return nullptr;
}

int Actor::AddCustomAnimation(std::unique_ptr<Animation>&& animation)
{
	if (animation) {
		actor_drawable_storage.emplace_back(std::move(animation), true, DrawableType::Animation);
		return actor_drawable_storage.size() - 1;
	}
	lActor.error() << "nullptr in Actor::AddAnimation()";
	return -1;
}

sf::Vector2f Actor::getMaxSize() {
	if (actor_drawable_storage.empty()) return {0, 0};
	float maxX = 0, maxY = 0;
	for (auto& obj : actor_drawable_storage) {
		if (obj.type == DrawableType::Sprite) {
			sf::Sprite* sprite_ptr = static_cast<sf::Sprite*>(obj.to_draw.get());
			auto size_ = sprite_ptr->getTexture()->getSize();
			auto size = sf::Vector2f(size_.x, size_.y);
			size.x *= sprite_ptr->getScale().x;
			size.y *= sprite_ptr->getScale().y;
			if (maxX < size.x) maxX = size.x;
			if (maxY < size.y) maxY = size.y;
		}
	}
	return {maxX, maxY};
}

sf::FloatRect Actor::getBounds(int obj_id)
{
	sf::FloatRect bounds;
	if (obj_id < 0 || obj_id >= static_cast<int>(actor_drawable_storage.size())) {
		lActor.error() << "Invalid place_id: " << obj_id << " in getGlobalBounds!" << std::endl;
		return bounds;
	}
	DrawableObj& drawable_obj = actor_drawable_storage[obj_id];
	switch (drawable_obj.type) {
		case DrawableType::Sprite: {
			auto* sprite = dynamic_cast<sf::Sprite*>(drawable_obj.to_draw.get());
			if (sprite) {
				bounds = sprite->getGlobalBounds();
			}
			else {
				lActor.error() << "Failed to cast to sf::Sprite* in getGlobalBounds!" << std::endl;
			}
			break;
		}
		case DrawableType::Text: {
			auto* text = dynamic_cast<sf::Text*>(drawable_obj.to_draw.get());
			if (text) {
				bounds = text->getGlobalBounds();
			}
			else {
				lActor.error() << "Failed to cast to TextComponent* in getGlobalBounds!" << std::endl;
			}
			break;
		}
		case DrawableType::VertexArray: {
			auto* shape = dynamic_cast<sf::VertexArray*>(drawable_obj.to_draw.get());
			if (shape) {
				bounds = shape->getBounds();
			}
			else {
				lActor.error() << "Failed to cast to sf::VertexArray* in getGlobalBounds!" << std::endl;
			}
			break;
		}
		default:
			lActor.error() << "Unsupported DrawableType in getGlobalBounds!" << std::endl;
			break;
	}

	return bounds;
}

sf::Time Actor::get_elapsed_time() const {
	return data::elapsedTime;
}

bool Actor::is_drawable_object_exists(int obj_id)
{
	if (obj_id >= 0 && obj_id < actor_drawable_storage.size() && actor_drawable_storage[obj_id].to_draw) {
		return true;
	}
	return false;
}

bool Actor::set_visibility(bool visibility, int obj_id)
{
	if (obj_id >= 0 && obj_id < actor_drawable_storage.size()) {
		actor_drawable_storage[obj_id].is_visible = visibility;
		return true;
	}
	return false;
}

bool Actor::set_visibility(bool visibility)
{
	if (actor_drawable_storage.empty()) {
		lActor.start_entry() << "set_visibility() with empty drawable storage!!";
		return false;
	}
	for (auto& val : actor_drawable_storage) {
		val.is_visible = visibility;
	}
	return true;
}

void Actor::on_delete() {
	if (actor_saving) {
		actor_saving->delete_data();
	}
	delete_record();
}

Actor::~Actor()
{
	if (!is_ui) {
		DrawableStorage::check_out(ActorIndex, prior);
	}
	else {
		DrawableStorage::check_out_ui(this);
	}
	delete actor_saving;
};

template void Actor::on_gui_item<sf::Sprite>(const DrawableObj& obj, const std::string& type, size_t obj_id);
template void Actor::on_gui_item<sf::Text>(const DrawableObj& obj, const std::string& type, size_t obj_id);