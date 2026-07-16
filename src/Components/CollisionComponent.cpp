#include "Components/CollisionComponent.h"

#include "EngineFiles/CollisionManager.hpp"
#include "BaseClasses/Actor.h"
#include "Utils/Logger.h"
#include "Utils/UsefulFunctions.h"
#include "UI/DefaultInterfaceTemplates.hpp"

#include "imgui.h"

static Logger lCC("CollisionComponent.log");


sf::FloatRect CollisionComponent::calculateInternalBounds() const
{
	auto maxSize = actor->getMaxSize();
	return {0, 0, maxSize.x, maxSize.y};
}

void CollisionComponent::initialization_of_collision(const sf::FloatRect collision_bounds)
{
	boxCollision = new sf::FloatRect();

	*boxCollision = collision_bounds;

	CollisionManager::check_in(this, type);
	is_on = true;
}

void CollisionComponent::initialization_of_collision_painting() {
	if (is_on && !is_collision_visible) {
		sf::VertexArray va(sf::LineStrip, 5);
		for (int i = 0; i != 5; ++i) {
			va[i].color = sf::Color::Red;
		}
		obj_id = actor->AddVertexArray(va);
		is_collision_visible = true;
		re_check_collision_painting();
	}
}

void CollisionComponent::re_check_collision_painting() const
{
	if (is_collision_visible && obj_id != -1) {
		sf::VertexArray* vaCollision = actor->get_va(obj_id);
		if (vaCollision != nullptr) {
			auto pos = boxCollision->getPosition();
			auto size = boxCollision->getSize();

			(*vaCollision)[0].position = pos;
			(*vaCollision)[1].position = sf::Vector2f(pos.x + size.x, pos.y);
			(*vaCollision)[2].position = sf::Vector2f(pos.x + size.x, pos.y + size.y);
			(*vaCollision)[3].position = sf::Vector2f(pos.x, pos.y + size.y);
			(*vaCollision)[4].position = pos;
		}
		else {
			lCC.error() << "trouble with re_check_collision_painting(), ptr == nullptr, obj_id: " << obj_id;
		}
	}
}

//Turning Collision on/off
void CollisionComponent::TurnOnCollision(CollisionType type_) {
	if (!is_on) {
		type = type_;
		if (boxCollision == nullptr) {
			initialization_of_collision(calculateInternalBounds());
		} else {
			CollisionManager::check_in(this, type);
			is_on = true;
			if (obj_id != -1) {
				make_collision_visible();
			}
		}
	}
}

void CollisionComponent::TurnOffCollision() {
	if (is_on) {
		CollisionManager::check_out(this);
		if (is_collision_visible) {
			make_collision_visible();
		}
		is_on = false;
	}
}

void CollisionComponent::AddToCollisionBoxSize(float x, float y) {
	if (is_on) {
		auto pos = boxCollision->getPosition();
		auto size = boxCollision->getSize();
		float x_div = x / 2.f;
		float y_div = y / 2.f;

		pos.x -= x_div;
		pos.y -= y_div;
		size.x += x;
		size.y += y;

		*boxCollision = sf::FloatRect(pos, size);

		re_check_collision_painting();

		if (type == CollisionType::Static) CollisionManager::update_bounds(this, recalc_globalBounds());
	}
}

void CollisionComponent::SetCollisionBoxSize(float x, float y) {
	*boxCollision = sf::FloatRect(boxCollision->getPosition(), {x, y});
	re_check_collision_painting();
	if (type == CollisionType::Static) CollisionManager::update_bounds(this, recalc_globalBounds());
}

void CollisionComponent::SetCollisionBoxPosition(float x, float y) {
	*boxCollision = sf::FloatRect({x, y}, boxCollision->getSize());
	re_check_collision_painting();
	if (type == CollisionType::Static) CollisionManager::update_bounds(this, recalc_globalBounds());
}

//It's needed for making your collision box visible
void CollisionComponent::make_collision_visible()
{
	if (is_on) {
		if (!is_collision_visible) {
			if (obj_id != -1) {
				actor->set_visibility(true, obj_id);
				is_collision_visible = true;
			} else {
				initialization_of_collision_painting();
			}
		}
		else {
			actor->set_visibility(false, obj_id);
			is_collision_visible = false;
		}
	}
}

void CollisionComponent::make_collision_event(CollisionComponent* other)
{
	on_collision(other->actor, other);
	other->on_collision(actor, this);
}

void CollisionComponent::on_gui() {
	ImGui::BulletText("Collision Component info:");
	bool c_visible = is_collision_visible;
	if (ImGui::Checkbox("Collision box visibility", &c_visible)) {
		make_collision_visible();
	}

	auto collisionType = CollisionManager::get_type(this);
	if (collisionType == CollisionType::Static) {
		ImGui::Text("Collision type = Static");
	}
	if (collisionType == CollisionType::Dynamic) {
		ImGui::Text("Collision type = Dynamic");
	}

	if (boxCollision) {
		auto table = DI::DefaultTable2Columns("Collision table");

		table.new_row("Collision box pos:");
		auto cb_pos = boxCollision->getPosition();
		if (ImGui::DragFloat2("##cb_position", cb_pos)) {
			SetCollisionBoxPosition(cb_pos.x, cb_pos.y);
		}

		table.new_row("Collision box size:");
		auto cb_size = boxCollision->getSize();
		if (ImGui::DragFloat2("##cb_size", cb_size)) {
			SetCollisionBoxSize(cb_size.x, cb_size.y);
		}

		table.new_row("Add to size & pos:");
		static float stepping = 5.f;
		if (ImGui::SmallButton("X")) {
			AddToCollisionBoxSize(stepping, 0.f);
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("Y")) {
			AddToCollisionBoxSize(0.f, stepping);
		}
		ImGui::SameLine();
		ImGui::DragFloat("##stepping", &stepping, 1.f);
	}
}

sf::FloatRect CollisionComponent::getCollisionBoxBoundsGlobal() const {
	return recalc_globalBounds();
}

sf::Vector2f CollisionComponent::getCollisionBoxCenterPositionGlobal() const {
	auto pos = actor->getPosition();
	auto size = boxCollision->getSize();
	auto scale = actor->getScale();
	size.x *= scale.x;
	size.y *= scale.y;
	return { pos.x + size.x / 2.f, pos.y + size.y / 2.f };
}

sf::Vector2f CollisionComponent::getCollisionBoxLocalSize() const {
	return boxCollision->getSize();
}

sf::Vector2f CollisionComponent::getCollisionBoxGlobalSize() const {
	auto scale = actor->getScale();
	auto size = boxCollision->getSize();
	return {size.x * scale.x, size.y * scale.y};
}

sf::FloatRect CollisionComponent::recalc_globalBounds() const {
	auto scale = actor->getScale();
	if (scale.x < 0.f) scale.x *= -1.f;
	if (scale.y < 0.f) scale.y *= -1.f;

	auto size = boxCollision->getSize();
	auto pos = boxCollision->getPosition();
	return {actor->getPosition() + sf::Vector2f(pos.x * scale.x, pos.y * scale.y),
		{size.x * scale.x, size.y * scale.y}};
}

CollisionComponent::~CollisionComponent() {
	delete boxCollision;
	if (is_on) {
		CollisionManager::check_out(this);
	}
}