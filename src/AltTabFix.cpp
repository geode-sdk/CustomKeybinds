#include <cocos2d.h>
#include <Geode/modify/AppDelegate.hpp>

#ifdef GEODE_IS_WINDOWS
class $modify(AppDelegate) {
    void applicationWillEnterForeground() {
        AppDelegate::applicationWillEnterForeground();
        cocos2d::CCDirector::get()->getKeyboardDispatcher()->updateModifierKeys(false, false, false, false);
    }
};
#endif
