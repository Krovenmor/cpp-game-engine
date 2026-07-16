#include "Components/AnimationComponent.h"
#include "BaseClasses/Actor.h"

#include "EngineFiles/Animation/Animation.hpp"
#include "EngineFiles/Animation/AnimationTileSet.hpp"
#include "EngineFiles/Animation/AnimationTextures.hpp"
#include "EngineFiles/Animation/AnimationUpdate.hpp"

#include "EngineFiles/Saving/SavingStructs/AnimationComponent_SS.hpp"

static Logger lAC("AnimationComponent.log");

void AnimationComponent::switch_animation(size_t index) {
	auto& anim = animations[current_animation_index];
	auto& new_anim = animations[index];
	actor_->set_visibility(false, anim->animation_index);
	actor_->set_visibility(true, new_anim->animation_index);
	current_animation = actor_->get_anim(new_anim->animation_index);
	current_animation_index = index;
	current_animation_playtime = sf::Time::Zero;
}

void AnimationComponent::init_saving() {
	if (!saving_component) {
		saving_component = new SavingComponent<AnimationComponentConfig, AnimationComponent>(CONFIG_AC_PATH);
	}
}

void AnimationComponent::check_and_switch() {
	auto& anim = animations[current_animation_index];
	size_t new_index = current_animation_index;

	if (auto ptr = anim->callback_ptr) {
		new_index = (*ptr)();
	}

	if (new_index != current_animation_index) {
		switch_animation(new_index);
	}
}

AnimationComponent::AnimationComponent(Actor *actor_ptr) : actor_(actor_ptr) {
	AnimComponentIndex = AnimationUpdate::add_animation_component(this);
}

void AnimationComponent::AnnounceAnimation(const char* animation_name, AnimationType type, func* callback) {
	init_saving();
	auto& tilesets_data = saving_component->get_config().tilesets_data;
	auto it = tilesets_data.find(animation_name);
	if (it == tilesets_data.end()) {
		lAC.error() << "no animation found in tilesets_anim_data!, with name: " << animation_name;
	}
}

void AnimationComponent::AddAnimation(std::unique_ptr<Animation> &&animation, AnimationType type, func* callback) {
	if (animation) {
		animation->turn_update(false);
		sf::Time duration = animation->get_duration();
		int index = actor_->AddCustomAnimation(std::move(animation));
		if (index != -1) {
			auto* new_anim_struct = new AnimStruct;
			new_anim_struct->animation_index = index;
			new_anim_struct->duration = duration;
			new_anim_struct->type = type;

			if (callback) {
				new_anim_struct->callback_ptr = new func(*callback);
			}

			animations.emplace_back(new_anim_struct);
			if (!current_animation) {
				current_animation = actor_->get_anim(index);
				current_animation_index = 0;
			} else {
				actor_->set_visibility(false, index);
			}
		}
	}
}

bool AnimationComponent::AddAnimationTileSet(const sf::Time deltaTime, const char *tilesetName,
	AnimationType type, func* callback) {

	auto anim = std::make_unique<AnimationTileSet>(false);
	if (anim->LoadAnimation(deltaTime, tilesetName)) {
		AddAnimation(std::move(anim), type, callback);
		return true;
	}
	return false;
}

bool AnimationComponent::AddAnimationTextures(sf::Time deltaTime, const std::vector<std::string> &texture_paths,
	AnimationType type, func* callback) {

	auto anim = std::make_unique<AnimationTextures>(false);
	if (anim->MakeAnimation(deltaTime, texture_paths)) {
		AddAnimation(std::move(anim), type, callback);
		return true;
	}
	return false;
}

void AnimationComponent::on_gui() {
	static bool is_opened = false;
	static int selected_index = -1;
	static Animation* selected_anim = nullptr;

	const float height = 100.f;
	const float max_width = 800.f;
	float width = ImGui::GetContentRegionAvail().x;
	width = std::min(width, max_width);

	ImGui::Separator();
	ImGui::BulletText("%s", "All animations:");
	if (ImGui::BeginTable("SelectionTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg
		| ImGuiTableFlags_ScrollY, ImVec2(width, height))) {

		ImGui::TableSetupColumn("AnimIndex");
		ImGui::TableSetupColumn("AnimationType");
		ImGui::TableSetupColumn("Duration");
		ImGui::TableSetupColumn("Is Callback");
		ImGui::TableHeadersRow();

		for (int i = 0; i < animations.size(); ++i) {
			const auto& anim = animations[i];

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			bool is_selected = (selected_index == i);
			if (ImGui::Selectable(std::to_string(i).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
				selected_index = i;
				selected_anim = actor_->get_anim(animations[selected_index]->animation_index);
				is_opened = true;
			}

			ImGui::TableSetColumnIndex(1);

			if (anim->type == AnimationType::Looping) {
				ImGui::Text("Looping");
			}
			if (anim->type == AnimationType::One_shot) {
				ImGui::Text("One shot");
			}

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%s", std::to_string(anim->duration.asSeconds()).c_str());

			ImGui::TableSetColumnIndex(3);
			ImGui::Text("%s", (anim->callback_ptr) ? "YES" : "NO");
		}
		ImGui::EndTable();
	}
	if (selected_index == -1) {
		ImGui::TextDisabled("Select an animation to inspect");
	}
	if (selected_anim && selected_index >= 0 && selected_index < animations.size() && is_opened) {
		ImGui::Begin("Animation Inspector", &is_opened);
		selected_anim->OnGui();
		ImGui::End();
	} else {
		selected_index = -1;
		selected_anim = nullptr;
	}
	ImGui::Separator();
}

void AnimationComponent::on_animIndex_change(size_t index) {
	AnimComponentIndex = index;
}

void AnimationComponent::check_and_update(const sf::Time elapsed) {
	if (!animations.empty()) {
		if (current_animation_playtime == sf::Time::Zero) {
			current_animation->from_start();
		}

		auto& anim = animations[current_animation_index];
		current_animation_playtime += elapsed;
		current_animation->check_and_update(elapsed);

		if (anim->type == AnimationType::One_shot) {
			if (current_animation_playtime >= anim->duration) {
				check_and_switch();
			}
		}
	}
}

AnimationComponent::~AnimationComponent() {
	for (const auto& i : animations) {
		delete i->callback_ptr;
		delete i;
	}
	delete saving_component;
	AnimationUpdate::delete_animation_component(AnimComponentIndex);
}
