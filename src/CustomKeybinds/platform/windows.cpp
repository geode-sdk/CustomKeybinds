#ifdef GEODE_IS_WINDOWS

#include <Geode/modify/CCEGLView.hpp>
#include <Geode/modify/Modify.hpp>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDispatcher.h>

#include <Keybinds.hpp>

using namespace geode::prelude;
using namespace geode::keybinds;

class $modify(CCEGLView) {
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
