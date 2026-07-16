#pragma once
#include "Actor.h"

#define DEFAULT_WIDGET_NAME "WidgetActor"

enum class Anchor {
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	Center
};

class WidgetActor : public Actor {
private:
	
	float offset_x = 0.f;
	float offset_y = 0.f;
	Anchor current_anchor;

	bool is_offset = false;
	void set_offset(const float& win_w, const float& win_h);

public:

	WidgetActor(Anchor anchor) : Actor(DEFAULT_WIDGET_NAME, true), current_anchor(anchor) {}
	WidgetActor(const char* widget_name, Anchor anchor) : Actor(widget_name, true), current_anchor(anchor) {}

	void set_default_position(float win_w, float win_h, float offset_x = 0.f, float offset_y = 0.f);

	// Reset offset to recalculate it on the next resize
	void reset_anchor(Anchor new_anchor);

	// This func will be called in game loop, when window is resized
	void on_resized(float new_width, float new_height, const float& win_w, const float& win_h);

	void debug_info();
};



