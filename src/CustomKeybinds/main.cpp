#include <string>

#include <matjson.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/PlatformToolbox.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/loader/Event.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/Setting.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/platform/cplatform.h>
#include <Geode/ui/Notification.hpp>
#include <Geode/DefaultInclude.hpp>
#include <Geode/Result.hpp>

#include <Keybinds.hpp>
#include <CustomKeybinds/DummySetting.hpp>
#include <CustomKeybinds/ui/KeybindsLayer.hpp>

using namespace geode::prelude;
using namespace geode::keybinds;

class ControllerChecker : public CCObject {
protected:
	bool m_cached = false;

public:
	void checkController(float) {
		// TODO: remove the android thingy once/if zmx figures out how to get the controller state through the launcher
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

$execute {
	(void) Mod::get()->registerCustomSettingType("open-menu", &DummySetting::parse);

	globalListen([](auto) {
		// check every second if a controller has been connected
		CCScheduler::get()->scheduleSelector(
			schedule_selector(ControllerChecker::checkController),
			new ControllerChecker(), 1.f, false
		);
	}, GameEventFilter(GameEventType::Loaded));
};