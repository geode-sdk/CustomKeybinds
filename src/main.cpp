#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include <Geode/binding/AppDelegate.hpp>
#include <Geode/ui/Notification.hpp>
#include "../include/Keybinds.hpp"
#include "KeybindsLayer.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(CCKeyboardDispatcher) {
	static inline std::unordered_set<enumKeyCodes> s_held {};

	static void onModify(auto& self) {
		(void)self.setHookPriority("cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG", 100000);
	}

	void updateModifierKeys(bool shift, bool ctrl, bool alt, bool cmd) {
		m_bShiftPressed = shift;
		m_bControlPressed = ctrl;
		m_bAltPressed = alt;
		m_bCommandPressed = cmd;
	}

	bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
		if (keyIsController(key)) {
			if (PressBindEvent(ControllerBind::create(key), down).post() == ListenerResult::Stop) {
				return true;
			}
		} else {
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
						// we want to pass modifiers onwards to the original
						break;
					}
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

class ControllerChecker : public CCObject {
protected:
	bool m_cached = false;

public:
	void checkController(float) {
		if (m_cached != AppDelegate::get()->getControllerConnected()) {
			m_cached = AppDelegate::get()->getControllerConnected();
			if (m_cached) {
				BindManager::get()->attachDevice("controller"_spr, &ControllerBind::parse);
				Notification::create(
					"Controller Attached",
					CCSprite::createWithSpriteFrameName("controllerBtn_A_001.png")
				)->show();
			}
			else {
				BindManager::get()->detachDevice("controller"_spr);
				Notification::create(
					"Controller Detached",
					CCSprite::createWithSpriteFrameName("controllerBtn_B_001.png")
				)->show();
			}
		}
	}

	ControllerChecker() {
		this->retain();
	}
};

$execute {
	// check every second if a controller has been connected
	Loader::get()->queueInMainThread([] {
		CCScheduler::get()->scheduleSelector(
			schedule_selector(ControllerChecker::checkController),
			new ControllerChecker(), 1.f, false
		);
	});
}
