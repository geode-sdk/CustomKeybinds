#pragma once

#include <memory>
#include <string>

#include <matjson.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/loader/Setting.hpp>
#include <Geode/Result.hpp>

class DummySetting : public geode::SettingBaseValue<int> {
public:
	static geode::Result<std::shared_ptr<SettingV3>> parse(
        std::string const&,
        std::string const&,
        matjson::Value const&
    ) {
		return Ok(std::make_shared<DummySetting>());
	};
	geode::SettingNode* createNode(float width) override;
};

class ButtonSettingNode : public geode::SettingValueNode<DummySetting> {
protected:
	bool init(std::shared_ptr<DummySetting>& setting, float width);
    void onOpen(cocos2d::CCObject*);

public:
	static ButtonSettingNode* create(std::shared_ptr<DummySetting> value, float width) {
		auto ret = new ButtonSettingNode();
		if (ret->init(value, width)) {
			ret->autorelease();
			return ret;
		}

        delete ret;
		return nullptr;
	}
};