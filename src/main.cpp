#include <Geode/loader/GameEvent.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

#ifndef GEODE_IS_IOS
class $modify(MoreOptionsLayer) {
	void onKeybindings(CCObject*) {
		openSettingsPopup(Mod::get(), false);
	}
};
#endif

$on_game(Loaded) {
	auto mod = Mod::get();
	if (!mod->getSavedValue<bool>("migrated-keybinds")) {
		mod->setSavedValue("migrated-keybinds", true);
		for (auto& [key, value] : mod->getSaveContainer()) {
			if (key.starts_with("robtop.geometry-dash/")) {
				std::vector<Keybind> binds;
				for (auto& bind : value["binds"]) {
					if (auto keybind = bind.as<Keybind>()) {
						binds.push_back(keybind.unwrap());
					}
					else {
						log::warn("Failed to parse bind for action {}: {}", key, keybind.unwrapErr());
					}
				}
				mod->setSettingValue(key.substr(22), binds);
			}
		}
	}
}
