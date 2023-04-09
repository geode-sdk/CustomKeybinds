#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include "../include/Keybinds.hpp"
#include "KeybindsLayer.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
		if (!keyIsModifier(key)) {
			Modifier modifiers = Modifier::None;
			if (m_bControlPressed) {
				modifiers |= Modifier::Control;
			}
			if (m_bAltPressed) {
				modifiers |= Modifier::Alt;
			}
			if (m_bCommandPressed) {
				modifiers |= Modifier::Command;
			}
			if (m_bShiftPressed) {
				modifiers |= Modifier::Shift;
			}
			if (PressBindEvent(Keybind::create(key, modifiers), down).post() == ListenerResult::Stop) {
				return true;
			}
		}
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
	}
};

class $modify(MoreOptionsLayer) {
	void onKeybindings(CCObject*) {
		KeybindsLayer::create()->show();
	}
};

$on_mod(Loaded) {
	BindManager::get()->addBindTo("robtop.geometry-dash/jump-p1", Keybind::create(enumKeyCodes::KEY_A, Modifier::None));
}
