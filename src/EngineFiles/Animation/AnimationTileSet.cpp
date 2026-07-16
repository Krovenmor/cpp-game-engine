#include "EngineFiles/Animation/AnimationTileSet.hpp"
#include "EngineFiles/TileSetContainer.h"

#include "Utils/Logger.h"

#include "imgui.h"
#include "UI/DefaultInterfaceTemplates.hpp"


bool AnimationTileSet::InitAnimation(sf::Time deltaTime) {
    if (sf::VertexArray* va = tileset->getVertexArray()) {
        size_t ts_size = tileset->getFramesCount();
        sf::Texture* texture_ptr = tileset->getTexture();
        auto& vAnimFrames = get_anim_frames();
        vAnimFrames.reserve(ts_size);
        for (size_t i = 0; i < ts_size; i++) {
            sf::VertexArray va_ = tileset->make_tile_FromTileSet(i, va);
            vAnimFrames.emplace_back(deltaTime, va_, texture_ptr);
        }
        return true;
    }
    lA.error() << "AnimationTileSet::InitAnimation: va is nullptr!!!" << std::endl;
    return false;
}

bool AnimationTileSet::InitAnimation(const std::vector<sf::Time>& deltaTimes) {
    size_t ts_size = tileset->getFramesCount();
    if (deltaTimes.size() == ts_size) {
        if (sf::VertexArray* va = tileset->getVertexArray()) {
            sf::Texture* texture_ptr = tileset->getTexture();
            auto& vAnimFrames = get_anim_frames();
            vAnimFrames.reserve(ts_size);
            for (size_t i = 0; i < ts_size; i++)
            {
                sf::VertexArray va_ = tileset->make_tile_FromTileSet(i, va);
                vAnimFrames.emplace_back(deltaTimes[i], va_, texture_ptr);
            }
            return true;
        }
        lA.error() << "AnimationTileSet::InitAnimation: va is nullptr!!!";
    } else {
        lA.error() << "AnimationTileSet::InitAnimation: you passed vector, that not equal to get_frames_count()";
    }
    return false;
}

bool AnimationTileSet::LoadAnimation(const char *tilesetName) {
    if (!tileset) {
        tileset = new TileSetContainer(tilesetName);
        if (tileset && tileset->load()) {
            return true;
        }
        lA.error() << "AnimationTileSet: LoadAnimation: trouble with load()" << std::endl;
    }
    return false;
}

void AnimationTileSet::onGui() {
    if (ImGui::CollapsingHeader("AnimationTileSet Settings")) {
        ImGui::SeparatorText("AnimationTileSet Settings");
        if (tileset) {
            const float indent_w = 10.f;

            ImGui::Indent(indent_w);
            tileset->on_gui_set();
            ImGui::Unindent(indent_w);
        } else {
            ImGui::TextColored(DI::RED(), "No tileset binded");
        }
    }
}

bool AnimationTileSet::LoadAnimation(sf::Time deltaTime, const char *tilesetName) {
    if (LoadAnimation(tilesetName)) {
        return InitAnimation(deltaTime);
    }
    return false;
}

bool AnimationTileSet::LoadAnimation(const std::vector<sf::Time> &deltaTimes, const char *tilesetName) {
    if (LoadAnimation(tilesetName)) {
        return InitAnimation(deltaTimes);
    }
    return false;
}

AnimationTileSet::~AnimationTileSet() {
    delete tileset;
}
