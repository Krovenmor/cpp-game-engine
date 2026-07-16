#include "CombinedUsefulClasses/TextWidget.h"
#include "EngineFiles/ObjectStorage.hpp"

bool TextWidget::set_font(const char *font_path) {
	if (text) {
		auto font = ObjectStorage::LoadFont(font_path);
		text->setFont(*font);
	}
	return false;
}
