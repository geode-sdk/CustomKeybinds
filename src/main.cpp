#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/binding/AppDelegate.hpp>
#include <Geode/binding/PlatformToolbox.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/modify/Modify.hpp>
#include <Geode/loader/Setting.hpp>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h>

#include <unordered_set>

#include "../include/Keybinds.hpp"
#include "KeybindsLayer.hpp"

using namespace geode::prelude;
using namespace keybinds;

#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCEGLView.hpp>

class $modify(CCEGLView){
	/**
	* GD does not pass shift into dispatchKeyboardMSG, causing the modifier to break when holding.
	* We need to manually pass in shift from onGLFWKeyCallback to resolve this bug.
	*/
	void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		enumKeyCodes keycode = enumKeyCodes::KEY_Unknown;

		switch (key) {
			case GLFW_KEY_LEFT_SHIFT:
				keycode = enumKeyCodes::KEY_LeftShift;
				break;
			case GLFW_KEY_RIGHT_SHIFT:
				keycode = enumKeyCodes::KEY_RightShift;
				break;
		}

		if (keycode != enumKeyCodes::KEY_Unknown) {
			CCKeyboardDispatcher::get()->dispatchKeyboardMSG(keycode, action >= 1, action == 2);
		}
		CCEGLView::onGLFWKeyCallback(window, key, scancode, action, mods);
	}

	void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int mods) {
		std::optional<MouseButton> mb;
		switch (button) {
			case 3: mb = MouseButton::Button4; break;
			case 4: mb = MouseButton::Button5; break;
			default: break;
		}
		if (mb) {
			Modifier modifiers = Modifier::None;
			if (mods & GLFW_MOD_SHIFT) {
				modifiers |= Modifier::Shift;
			}
			if (mods & GLFW_MOD_ALT) {
				modifiers |= Modifier::Alt;
			}
			if (mods & GLFW_MOD_CONTROL) {
				modifiers |= Modifier::Control;
			}
			if (auto bind = MouseBind::create(*mb, modifiers)) {
				if (PressBindEvent(bind, action == GLFW_PRESS).post() == ListenerResult::Stop) {
					return;
				}
			}
		}
		else {
			return CCEGLView::onGLFWMouseCallBack(window, button, action, mods);
		}
	}
};
#endif

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
		//TODO: remove the android thingy once/if zmx figures out how to get the controller state through the launcher
		bool controllerConnected = GEODE_ANDROID(true || ) PlatformToolbox::isControllerConnected();
		if (m_cached != controllerConnected) {
			m_cached = controllerConnected;
			if (m_cached) {
				BindManager::get()->attachDevice("controller"_spr, &ControllerBind::parse);
				#ifndef GEODE_IS_ANDROID
				Notification::create(
					"Controller Attached",
					CCSprite::createWithSpriteFrameName("controllerBtn_A_001.png")
				)->show();
				#endif
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

bool startedChecker = false;

class $modify(CKMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		if (startedChecker) return true;

		// check every second if a controller has been connected
		Loader::get()->queueInMainThread([] {
			CCScheduler::get()->scheduleSelector(
				schedule_selector(ControllerChecker::checkController),
				new ControllerChecker(), 1.f, false
			);
			});

		startedChecker = true;

		return true;
	}
};

// Have to make a SettingValue even if it holds no value
class DummySetting : public SettingBaseValue<int> {
public:
	static Result<std::shared_ptr<SettingV3>> parse(std::string const&, std::string const&, matjson::Value const&) {
		return Ok(std::make_shared<DummySetting>());
	};
	SettingNode* createNode(float width) override;
};

class ButtonSettingNode : public SettingValueNode<DummySetting> {
protected:
	bool init(std::shared_ptr<DummySetting>& setting, float width) {
		if (!SettingValueNodeV3::init(setting, width))
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
	void updateState(CCNode* invoker) override {
		SettingValueNodeV3::updateState(invoker);
	}

	static ButtonSettingNode* create(std::shared_ptr<DummySetting> value, float width) {
		auto ret = new ButtonSettingNode();
		if (ret && ret->init(value, width)) {
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};

SettingNode* DummySetting::createNode(float width) {
	return ButtonSettingNode::create(std::static_pointer_cast<DummySetting>(shared_from_this()), width);
}

$execute {
	(void) Mod::get()->registerCustomSettingType("open-menu", &DummySetting::parse);
}
