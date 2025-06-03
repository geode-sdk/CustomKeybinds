#include <CustomKeybinds/DummySetting.hpp>

#include <memory>

#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/loader/SettingV3.hpp>

#include <CustomKeybinds/ui/KeybindsLayer.hpp>

using namespace geode::prelude;

SettingNode* DummySetting::createNode(float width) {
	return ButtonSettingNode::create(std::static_pointer_cast<DummySetting>(shared_from_this()), width);
}

bool ButtonSettingNode::init(std::shared_ptr<DummySetting>& setting, float width) {
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

void ButtonSettingNode::onOpen(CCObject*) {
    KeybindsLayer::create()->show();
}
