#include <SFML/Graphics/Texture.hpp>
#include "EngineFiles/Animation/AnimationTextures.hpp"
#include "EngineFiles/ObjectStorage.hpp"

#include "Utils/Logger.h"

#include "imgui.h"


sf::VertexArray AnimationTextures::make_va_frame_texture(const sf::Texture *texture) {
    sf::VertexArray va_return;
    va_return.setPrimitiveType(sf::PrimitiveType::Triangles);
    va_return.resize(6);

    auto size = texture->getSize();

    va_return[0].position = sf::Vector2f(0, 0);
    va_return[1].position = sf::Vector2f(size.x, 0);
    va_return[2].position = sf::Vector2f(0, size.y);
    //Second
    va_return[3].position = sf::Vector2f(0, size.y);
    va_return[4].position = sf::Vector2f(size.x, size.y);
    va_return[5].position = sf::Vector2f(size.x, 0);

    //First
    va_return[0].texCoords = sf::Vector2f(0, 0);
    va_return[1].texCoords = sf::Vector2f(size.x, 0);
    va_return[2].texCoords = sf::Vector2f(0, size.y);
    //Second
    va_return[3].texCoords = sf::Vector2f(0, size.y);
    va_return[4].texCoords = sf::Vector2f(size.x, size.y);
    va_return[5].texCoords = sf::Vector2f(size.x, 0);

    return va_return;
}

void AnimationTextures::onGui() {
    if (ImGui::CollapsingHeader("AnimationTextures Settings")) {
        ImGui::SeparatorText("AnimationTextures Settings");
    }
}

bool AnimationTextures::MakeAnimation(sf::Time deltaTime, const std::vector<std::string> &texture_paths) {
    size_t textures_count = texture_paths.size();
    if (textures_count >= 2) {
        auto& vAnimFrames = get_anim_frames();
        vAnimFrames.reserve(textures_count);
        for (size_t i = 0; i < textures_count; i++) {
            auto texture_ptr = ObjectStorage::LoadTexture(texture_paths[i]);
            if (texture_ptr) {
                sf::VertexArray va_ = make_va_frame_texture(texture_ptr);
                vAnimFrames.emplace_back(deltaTime, va_, texture_ptr);
            } else {
                lA.error() << "AnimationTextures::MakeAnimation(deltaTime): Trouble with texture!";
                return false;
            }
        }
        return true;
    }
    lA.error() << "AnimationTextures::MakeAnimation: We can't make animation on 1 texture!";
    return false;
}

bool AnimationTextures::MakeAnimation(const std::vector<sf::Time> &deltaTimes,
    const std::vector<std::string> &texture_paths) {
    size_t textures_count = texture_paths.size();
    if (textures_count >= 2) {
        if (textures_count != deltaTimes.size()) {
            auto& vAnimFrames = get_anim_frames();
            vAnimFrames.reserve(textures_count);
            for (size_t i = 0; i < textures_count; i++) {
                auto texture_ptr = ObjectStorage::LoadTexture(texture_paths[i]);
                if (texture_ptr) {
                    sf::VertexArray va_ = make_va_frame_texture(texture_ptr);
                    vAnimFrames.emplace_back(deltaTimes[i], va_, texture_ptr);
                } else {
                    lA.error() << "AnimationTextures::MakeAnimation(deltaTime): Trouble with texture!";
                    return false;
                }
            }
            return true;
        }
        lA.error() << "AnimationTextures::MakeAnimation: deltaTimes.size() != texture_paths.size()";
    } else {
        lA.error() << "AnimationTextures::MakeAnimation: We can't make animation on 1 texture!";
    }
    return false;
}