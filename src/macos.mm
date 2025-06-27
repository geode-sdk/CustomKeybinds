#include <Geode/Loader.hpp>
#include <Geode/Utils.hpp>

#ifdef GEODE_IS_MACOS
#define CommentType CommentTypeDummy
#import <Cocoa/Cocoa.h>
#include <objc/runtime.h>
#undef CommentType

#import <Geode/cocos/platform/mac/EAGLView.h>

using namespace geode::prelude;

void flagsChangedExecHook(EAGLView* self, SEL sel, NSEvent* event) {
	auto keyboardDispatcher = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher();

	auto shiftPressed = [event modifierFlags] & NSEventModifierFlagShift;
	if (keyboardDispatcher->m_bShiftPressed != shiftPressed) {
		keyboardDispatcher->dispatchKeyboardMSG(enumKeyCodes::KEY_Shift, shiftPressed, false);
	}

	auto altPressed = [event modifierFlags] & NSEventModifierFlagOption;
	if (keyboardDispatcher->m_bAltPressed != altPressed) {
		keyboardDispatcher->dispatchKeyboardMSG(enumKeyCodes::KEY_Alt, altPressed, false);
	}

	auto controlPressed = [event modifierFlags] & NSEventModifierFlagControl;
	if (keyboardDispatcher->m_bControlPressed != controlPressed) {
		keyboardDispatcher->dispatchKeyboardMSG(enumKeyCodes::KEY_Control, controlPressed, false);
	}

	auto commandPressed = [event modifierFlags] & NSEventModifierFlagCommand;
	if (keyboardDispatcher->m_bCommandPressed != commandPressed) {
		// there's not actually a command keycode so this might be bad
		keyboardDispatcher->dispatchKeyboardMSG(enumKeyCodes::KEY_Control, commandPressed, false);
	}

	// gd should call updateModifierKeys for us. not that i think it does but it should
	[self performSelector:sel withObject:event];
}

#define HOOK_OBJC_METHOD(klass, methodName) \
    auto methodName##Addr = class_getInstanceMethod(klass, @selector(methodName:)); \
    static_cast<void>(Mod::get()->hook(reinterpret_cast<void*>(method_getImplementation(methodName##Addr)), &methodName##Hook, #klass " " #methodName));

$execute {
	// yep, we're hooking cocoa girls

	auto eaglView = objc_getClass("EAGLView");
	HOOK_OBJC_METHOD(eaglView, flagsChangedExec);
}

#endif