#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include <Geode/binding/AppDelegate.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/modify/Modify.hpp>
#include <Geode/loader/SettingNode.hpp>
#include <Geode/loader/Setting.hpp>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h>

#include <unordered_set>

#include "../include/Keybinds.hpp"
#include "KeybindsLayer.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(CCKeyboardDispatcher) {
	static inline std::unordered_set<enumKeyCodes> s_held {};

	static void onModify(auto& self) {
		(void)self.setHookPriority("cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG", 1000);
	}

	void updateModifierKeys(bool shift, bool ctrl, bool alt, bool cmd) {
		m_bShiftPressed = shift;
		m_bControlPressed = ctrl;
		m_bAltPressed = alt;
		m_bCommandPressed = cmd;
	}

	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool p2) {
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
				// If no actual key was being held, just modifiers
				if (s_held.empty() && !down) {
					return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, p2);
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
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, p2);
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
		if (m_cached != PlatformToolbox::isControllerConnected()) {
			m_cached = PlatformToolbox::isControllerConnected();
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

// Have to make a SettingValue even if it holds no value
class DummySettingValue : public SettingValue {
public:
	DummySettingValue(std::string const& key, std::string const& mod) : SettingValue(key, mod) {}
	bool load(matjson::Value const& json) override { return true; }
	bool save(matjson::Value&) const override { return true; }
	SettingNode* createNode(float width) override;
};

class ButtonSettingNode : public SettingNode {
protected:
	bool init(SettingValue* value, float width) {
		if (!SettingNode::init(value))
			return false;

		this->setContentSize({ width, 40.f });

		auto* sprite = ButtonSprite::create("Edit Keys", 0, false, "bigFont.fnt", "GJ_button_04.png", 24.5f, 0.4f);
		auto* btn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(ButtonSettingNode::onOpen));
		auto* menu = CCMenu::create();
		menu->setPosition({ width / 2, 20.f });
		menu->addChild(btn);
		this->addChild(menu);

		return true;
	}
	void onOpen(CCObject*) {
		KeybindsLayer::create()->show();
	}
public:
	void commit() override { this->dispatchCommitted(); }
	bool hasUncommittedChanges() override { return false; }
	bool hasNonDefaultValue() override { return false; }
	void resetToDefault() override {}

	static ButtonSettingNode* create(SettingValue* value, float width) {
		auto ret = new ButtonSettingNode();
		if (ret && ret->init(value, width)) {
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};

SettingNode* DummySettingValue::createNode(float width) {
	return ButtonSettingNode::create(this, width);
}

$execute {
	Mod::get()->registerCustomSetting("open-menu", std::make_unique<DummySettingValue>(std::string("open-menu"), Mod::get()->getID()));
}
