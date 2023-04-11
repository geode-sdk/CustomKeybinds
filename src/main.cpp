#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include "../include/Keybinds.hpp"
#include "KeybindsLayer.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(CCKeyboardDispatcher) {
	static inline std::unordered_set<enumKeyCodes> s_held {};

	bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
		if (!keyIsModifier(key)) {
			if (down) {
				log::debug("hold {}", key);
				s_held.insert(key);
			}
			else {
				log::debug("release {}", key);
				s_held.erase(key);
			}
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
		// dispatch release events for Modifier + Key combos
		else {
			Modifier modifiers = Modifier::None;
			if (m_bControlPressed || key == KEY_Control) {
				modifiers |= Modifier::Control;
			}
			if (m_bAltPressed || key == KEY_Alt) {
				modifiers |= Modifier::Alt;
			}
			if (m_bCommandPressed /* todo */) {
				modifiers |= Modifier::Command;
			}
			if (m_bShiftPressed || key == KEY_Shift) {
				modifiers |= Modifier::Shift;
			}
			for (auto& held : s_held) {
				if (PressBindEvent(Keybind::create(held, modifiers), down).post() == ListenerResult::Stop) {
					return true;
				}
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
