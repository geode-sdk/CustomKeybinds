#include <Geode/modify/Modify.hpp>
#include <Geode/binding/MoreOptionsLayer.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>

#include <CustomKeybinds/ui/KeybindsLayer.hpp>

class $modify(MoreOptionsLayer) {
	$override
	void onKeybindings(CCObject*) {
		KeybindsLayer::create()->show();
	}
};
