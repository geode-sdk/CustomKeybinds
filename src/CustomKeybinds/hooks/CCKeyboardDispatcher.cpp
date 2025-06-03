#include <unordered_set>

#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h>
#include <Geode/loader/Event.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/Modify.hpp>

#include <Keybinds.hpp>

using namespace geode::prelude;
using namespace geode::keybinds;

class $modify(CCKeyboardDispatcher) {
	static inline std::unordered_set<enumKeyCodes> s_held {};

	static void onModify(auto& self) {
		(void)self.setHookPriority("cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG", Priority::Late);
	}

    $override
	void updateModifierKeys(bool shift, bool ctrl, bool alt, bool cmd) {
		m_bShiftPressed = shift;
		m_bControlPressed = ctrl;
		m_bAltPressed = alt;
		m_bCommandPressed = cmd;
	}

    $override
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat) {
		if (keyIsController(key)) {
			if (PressBindEvent(ControllerBind::create(key), down).post() == ListenerResult::Stop) {
				return true;
			}
		}
		else {
			if (!keyIsModifier(key)) {
				if (down) {
					s_held.insert(key);
				}
				else {
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
				if (auto bind = Keybind::create(key, modifiers)) {
					if (PressBindEvent(bind, down).post() == ListenerResult::Stop) {
						return true;
					}
				}
			}
			// dispatch release events for Modifier + Key combos
			else {
				bool stoppedRepeats = false;
				// Solves keybinds that keep repeating after letting go of the actual key
				if (s_held.size() > 0) {
					BindManager::get()->stopAllRepeats();
					stoppedRepeats = true;
				}
				// If no actual key was being held, just modifiers
				if (!down) {
					// Stop repeats here, resolves repeat issue when keys and modifiers are pressed in reverse
					if (!stoppedRepeats) {
						BindManager::get()->stopAllRepeats();
					}
					if (s_held.empty()) {
						return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat);
					}
				}
				std::unordered_set<Modifier> modifiersToToggle = this->getModifiersToToggle(key, down);
				bool ok = true;
				for (auto& held : s_held) {
					if (!ok) {
						break;
					}
					for (Modifier modifiers : modifiersToToggle) {
						Keybind* bind = Keybind::create(held, modifiers);
						if (bind && PressBindEvent(bind, down).post() == ListenerResult::Stop) {
							// we want to pass modifiers onwards to the original
							ok = false;
							break;
						}
					}
				}
			}
		}
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat);
	}

	/**
	 * Get the modifiers that need to be verified for keybinds. If a modifier is being released we need to check all combinations that use that modifier.
	 * @param key the modifier that was pressed
	 * @param down if the modifier was pressed or released
	 * @returns a list of modifiers that need to be checked for keybinds
	*/
	std::unordered_set<Modifier> getModifiersToToggle(enumKeyCodes key, bool down) {
		std::unordered_set<Modifier> modifiersToToggle = {};
		if (!keyIsModifier(key)) {
			return modifiersToToggle;	
		}
		if (down) {
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
			modifiersToToggle.insert(modifiers);
		} else {
			/**
			 * We need to disable all combinations that use the "disabled" modifier key.
			 * Left/Right variations don't seem to be used by GD.
			 * Adding them just to be sure.
			*/
			switch (key) {
				case KEY_LeftControl:
				case KEY_RightContol:
				case KEY_Control: {
					modifiersToToggle.insert(Modifier::Control);
					modifiersToToggle.insert(Modifier::Control | Modifier::Alt);
					modifiersToToggle.insert(Modifier::Control | Modifier::Shift);
					modifiersToToggle.insert(Modifier::Control | Modifier::Shift | Modifier::Alt);
					break;
				}
				case KEY_LeftShift:
				case KEY_RightShift:
				case KEY_Shift: {
					modifiersToToggle.insert(Modifier::Shift);
					modifiersToToggle.insert(Modifier::Shift | Modifier::Alt);
					modifiersToToggle.insert(Modifier::Control | Modifier::Shift);
					modifiersToToggle.insert(Modifier::Control | Modifier::Shift | Modifier::Alt);
					break;
				}
				case KEY_Alt: {
					modifiersToToggle.insert(Modifier::Alt);
					modifiersToToggle.insert(Modifier::Alt | Modifier::Control);
					modifiersToToggle.insert(Modifier::Alt | Modifier::Shift);
					modifiersToToggle.insert(Modifier::Control | Modifier::Shift | Modifier::Alt);
					break;
				}
				default: {
					// This shouldn't happen
					break;
				}
			}
		}
		return modifiersToToggle;
	}
};