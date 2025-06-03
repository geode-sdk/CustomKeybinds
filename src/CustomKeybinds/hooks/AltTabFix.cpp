#include <cocos2d.h>
#include <Geode/cocos/CCDirector.h>
#include <Geode/modify/AppDelegate.hpp>

using namespace geode::prelude;

#ifdef GEODE_IS_WINDOWS
// Fixes modifier keys still being pressed after returning from alt-tab
class $modify(AppDelegate) {
    void applicationWillEnterForeground() {
        AppDelegate::applicationWillEnterForeground();
        CCDirector::get()->getKeyboardDispatcher()->updateModifierKeys(false, false, false, false);
    }
};
#endif
