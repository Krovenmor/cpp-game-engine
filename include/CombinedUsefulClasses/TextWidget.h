#pragma once
#include "BaseClasses/WidgetActor.h"

class TextWidget : public WidgetActor {
public:

	int text_id = -1;
	sf::Text* text;

	void init () {
		text_id = AddText();
		text = static_cast<WidgetActor*>(this)->get_text(text_id);
	}

	bool set_font(const char* font_path);

	explicit TextWidget(const Anchor anchor) : WidgetActor(anchor) { init(); }
	explicit TextWidget(const char* name, const Anchor anchor) : WidgetActor(name, anchor) { init();}
};
