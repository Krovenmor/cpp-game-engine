#include "BaseClasses/WidgetActor.h"

void WidgetActor::set_default_position(float win_w, float win_h, float offset_x, float offset_y)
{
    sf::FloatRect bounds = getBounds(0);

    sf::Vector2f actor_scale = getScale();

    float display_width = bounds.width * actor_scale.x;
    float display_height = bounds.height * actor_scale.y;
	float edje_offset = 10.f;

    switch (current_anchor) {
    case Anchor::TopLeft:
        setPosition(offset_x, offset_y);
        break;
    case Anchor::TopRight:
        setPosition(win_w - display_width - offset_x - edje_offset, offset_y);
        break;
    case Anchor::BottomLeft:
        setPosition(offset_x, win_h - display_height - offset_y - edje_offset);
        break;
    case Anchor::BottomRight:
        setPosition(win_w - display_width - offset_x - edje_offset,
            win_h - display_height - offset_y - edje_offset);
        break;
    case Anchor::Center:
        setPosition((win_w - display_width) / 2.f + offset_x,
            (win_h - display_height) / 2.f + offset_y);
        break;
    }

    this->offset_x = 0.f;
    this->offset_y = 0.f;
    is_offset = false;
}

void WidgetActor::reset_anchor(Anchor new_anchor)
{
    current_anchor = new_anchor;
	is_offset = false;
}

void WidgetActor::set_offset(const float& win_w, const float& win_h)
{
    if (is_offset) return;

    auto pos = getPosition();
    sf::FloatRect bounds = getBounds(0);
    sf::Vector2f actor_scale = getScale();

    float obj_width = bounds.width * actor_scale.x;
    float obj_height = bounds.height * actor_scale.y;

    float obj_center_x = pos.x + obj_width / 2.f;
    float obj_center_y = pos.y + obj_height / 2.f;

    switch (current_anchor) {
    case Anchor::TopLeft:
        offset_x = pos.x;
        offset_y = pos.y;
        break;
    case Anchor::TopRight:
        offset_x = win_w - (pos.x + obj_width);
        offset_y = pos.y;
        break;
    case Anchor::BottomLeft:
        offset_x = pos.x;
        offset_y = win_h - (pos.y + obj_height);
        break;
    case Anchor::BottomRight:
        offset_x = win_w - (pos.x + obj_width);
        offset_y = win_h - (pos.y + obj_height);
        break;
    case Anchor::Center:
        offset_x = obj_center_x - (win_w / 2.f);
        offset_y = obj_center_y - (win_h / 2.f);
        break;
    }

    is_offset = true;
}

void WidgetActor::on_resized(float new_width, float new_height, const float& win_w, const float& win_h)
{
    set_offset(win_w, win_h);

    sf::FloatRect bounds = getBounds(0);
    sf::Vector2f actor_scale = getScale();

    float obj_width = bounds.width * actor_scale.x;
    float obj_height = bounds.height * actor_scale.y;

    float new_posX = 0.f, new_posY = 0.f;

    switch (current_anchor) {
    case Anchor::TopLeft:
        new_posX = offset_x;
        new_posY = offset_y;
        break;
    case Anchor::TopRight:
        new_posX = new_width - offset_x - obj_width;
        new_posY = offset_y;
        break;
    case Anchor::BottomLeft:
        new_posX = offset_x;
        new_posY = new_height - offset_y - obj_height;
        break;
    case Anchor::BottomRight:
        new_posX = new_width - offset_x - obj_width;
        new_posY = new_height - offset_y - obj_height;
        break;
    case Anchor::Center:
        new_posX = (new_width / 2.f) + offset_x - (obj_width / 2.f);
        new_posY = (new_height / 2.f) + offset_y - (obj_height / 2.f);
        break;
    }

    setPosition(new_posX, new_posY);
}

void WidgetActor::debug_info() {
    sf::FloatRect bounds = getBounds(0);
    sf::Vector2f scale = getScale();
    auto pos = getPosition();

    printf("=== Widget Debug ===\n");
    printf("Position: (%f, %f)\n", pos.x, pos.y);
    printf("Scale: (%f, %f)\n", scale.x, scale.y);
    printf("Original bounds: %fx%f\n", bounds.width, bounds.height);
    printf("Displayed size: %fx%f\n", bounds.width * scale.x, bounds.height * scale.y);
    printf("Anchor: %d\n", (int)current_anchor);
    printf("Offset: (%f, %f)\n", offset_x, offset_y);
    printf("==================\n");
}