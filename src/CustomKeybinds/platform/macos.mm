#ifdef GEODE_IS_MACOS

#include <Geode/Loader.hpp>
#include <Geode/Utils.hpp>

#define CommentType CommentTypeDummy
#import <Cocoa/Cocoa.h>
#include <objc/runtime.h>
#undef CommentType

using namespace geode::prelude;

static IMP s_originalSendEventIMP;

void sendEvent(NSApplication* self, SEL sel, NSEvent* event) {
	NSEventType type;

	type = [event type];
	if (type == NSEventTypeKeyUp) {
		switch (type) {
		case NSEventTypeKeyUp:
			[[[self mainWindow] firstResponder] tryToPerform:@selector(keyUp:) with:event ];
			return;
		default:
			break;
		}
	}

	((decltype(&sendEvent))s_originalSendEventIMP)(self, sel, event);
}

$execute {
	// yep, we're hooking cocoa girls

	auto method = class_getInstanceMethod(objc_getClass("NSApplication"), @selector(sendEvent:));
	s_originalSendEventIMP = method_getImplementation(method);
	method_setImplementation(method, (IMP)&sendEvent);
}

#endif