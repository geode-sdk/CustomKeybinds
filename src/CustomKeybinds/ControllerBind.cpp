#include <Keybinds.hpp>

using namespace geode::prelude;
using namespace geode::keybinds;

ControllerBind* ControllerBind::create(enumKeyCodes button) {
    if (!keyIsController(button)) {
        return nullptr;
    }
    auto ret = new ControllerBind();
    ret->m_button = button;
    ret->autorelease();
    return ret;
}

ControllerBind* ControllerBind::parse(matjson::Value const& value) {
    return ControllerBind::create(
        static_cast<enumKeyCodes>(value["button"].asInt().unwrapOr(0))
    );
}

matjson::Value ControllerBind::save() const {
    return matjson::makeObject({
        { "button", static_cast<int>(m_button) },
    });
}

enumKeyCodes ControllerBind::getButton() const {
    return m_button;
}

size_t ControllerBind::getHash() const {
    return m_button;
}

bool ControllerBind::isEqual(Bind* other) const {
    if (auto o = typeinfo_cast<ControllerBind*>(other)) {
        return m_button == o->m_button;
    }
    return false;
}

std::string ControllerBind::toString() const {
    return keyToString(m_button);
}

CCNode* ControllerBind::createLabel() const {
    const char* sprite;
    switch (m_button) {
        case CONTROLLER_A: sprite = "controllerBtn_A_001.png"; break;
        case CONTROLLER_B: sprite = "controllerBtn_B_001.png"; break;
        case CONTROLLER_X: sprite = "controllerBtn_X_001.png"; break;
        case CONTROLLER_Y: sprite = "controllerBtn_Y_001.png"; break;
        case CONTROLLER_Back: sprite = "controllerBtn_Back_001.png"; break;
        case CONTROLLER_Start: sprite = "controllerBtn_Start_001.png"; break;
        case CONTROLLER_Down: sprite = "controllerBtn_DPad_Down_001.png"; break;
        case CONTROLLER_Left: sprite = "controllerBtn_DPad_Left_001.png"; break;
        case CONTROLLER_Up: sprite = "controllerBtn_DPad_Up_001.png"; break;
        case CONTROLLER_Right: sprite = "controllerBtn_DPad_Right_001.png"; break;
        case CONTROLLER_LT: sprite = "controllerBtn_LT_001.png"_spr; break;
        case CONTROLLER_RT: sprite = "controllerBtn_RT_001.png"_spr; break;
        // todo: are these the same
        case CONTROLLER_LB: sprite = "controllerBtn_LB_001.png"_spr; break;
        case CONTROLLER_RB: sprite = "controllerBtn_RB_001.png"_spr; break;
        case CONTROLLER_LTHUMBSTICK_DOWN: sprite = "controllerBtn_LThumb_001.png"; break;
        case CONTROLLER_LTHUMBSTICK_LEFT: sprite = "controllerBtn_LThumb_001.png"; break;
        case CONTROLLER_LTHUMBSTICK_RIGHT: sprite = "controllerBtn_LThumb_001.png"; break;
        case CONTROLLER_LTHUMBSTICK_UP: sprite = "controllerBtn_LThumb_001.png"; break;
        case CONTROLLER_RTHUMBSTICK_RIGHT: sprite = "controllerBtn_RThumb_001.png"; break;
        case CONTROLLER_RTHUMBSTICK_DOWN: sprite = "controllerBtn_RThumb_001.png"; break;
        case CONTROLLER_RTHUMBSTICK_LEFT: sprite = "controllerBtn_RThumb_001.png"; break;
        case CONTROLLER_RTHUMBSTICK_UP: sprite = "controllerBtn_RThumb_001.png"; break;
        default: sprite = nullptr;
    }
    if (!sprite) {
        return CCLabelBMFont::create("Unk", "goldFont.fnt");
    }
    auto spr = CCSprite::createWithSpriteFrameName(sprite);
    switch (m_button) {
        case CONTROLLER_LTHUMBSTICK_DOWN:
        case CONTROLLER_RTHUMBSTICK_DOWN: {
            auto arrow = CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png");
            arrow->setPosition(ccp(13.f, -5.5f));
            arrow->setScale(0.7f);
            spr->addChild(arrow);
            break;
        }
        case CONTROLLER_LTHUMBSTICK_LEFT:
        case CONTROLLER_RTHUMBSTICK_LEFT: {
            auto arrow = CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png");
            arrow->setPosition(ccp(-5.5f, 13.5f));
            arrow->setScale(0.7f);
            arrow->setRotation(90.f);
            spr->addChild(arrow);
            break;
        }
        case CONTROLLER_LTHUMBSTICK_RIGHT:
        case CONTROLLER_RTHUMBSTICK_RIGHT: {
            auto arrow = CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png");
            arrow->setPosition(ccp(31.5f, 13.f));
            arrow->setScale(0.7f);
            arrow->setRotation(270.f);
            spr->addChild(arrow);
            break;
        }
        case CONTROLLER_LTHUMBSTICK_UP:
        case CONTROLLER_RTHUMBSTICK_UP: {
            auto arrow = CCSprite::createWithSpriteFrameName("PBtn_Arrow_001.png");
            arrow->setPosition(ccp(13.f, 31.f));
            arrow->setScale(0.7f);
            arrow->setRotation(180.f);
            spr->addChild(arrow);
            break;
        }

        default: {}
    }
    return spr;
}

std::string ControllerBind::getDeviceID() const {
    return "controller"_spr;
}
