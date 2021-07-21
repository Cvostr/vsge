#include "AngelApi.hpp"
#include "../AngelScriptLayer.hpp"
#include <Engine/WindowEvent.hpp>

using namespace VSGE;

void VSGE::BindInput() {
	AngelScriptLayer* layer = AngelScriptLayer::Get();

	layer->RegisterEnum(KEY_TYPE_NAME);
	for (char i = 0; i < 26; i++) {
		char letter = 'A' + i;
		std::string name = "KEY_";
		name.push_back(letter);
		layer->RegisterEnumValue(KEY_TYPE_NAME, name.c_str(), KEY_CODE_A + i);
	}
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_ESCAPE", KEY_CODE_ESCAPE);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_TAB", KEY_CODE_TAB);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_LCTRL", KEY_CODE_LCTRL);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_RCTRL", KEY_CODE_RCTRL);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_LSHIFT", KEY_CODE_LSHIFT);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_LALT", KEY_CODE_LALT);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_RALT", KEY_CODE_RALT);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_ENTER", KEY_CODE_ENTER);
	layer->RegisterEnumValue(KEY_TYPE_NAME, "KEY_SPACE", KEY_CODE_SPACE);
}