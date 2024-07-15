#include "../include/Keybinds.hpp"

using namespace geode::prelude;
using namespace keybinds;

MouseBind* MouseBind::create(MouseButton button, Modifier modifiers) {
    auto ret = new MouseBind();
    ret->m_button = button;
    ret->m_modifiers = modifiers;
    ret->autorelease();
    return ret;
}
MouseBind* MouseBind::parse(matjson::Value const& value) {
    return MouseBind::create(
        static_cast<MouseButton>(value["button"].as_int()),
        static_cast<Modifier>(value["modifiers"].as_int())
    );
}

MouseButton MouseBind::getButton() const {
    return m_button;
}
Modifier MouseBind::getModifiers() const {
    return m_modifiers;
}

size_t MouseBind::getHash() const {
    return static_cast<size_t>(m_button) | (static_cast<size_t>(m_modifiers) << 29);
}
bool MouseBind::isEqual(Bind* other) const {
    if (auto o = typeinfo_cast<MouseBind*>(other)) {
        return m_button == o->m_button && m_modifiers == o->m_modifiers;
    }
    return false;
}
std::string MouseBind::toString() const {
    std::string res = "";
    if (m_modifiers & Modifier::Control) {
        res += "Ctrl + ";
    }
    if (m_modifiers & Modifier::Command) {
        res += "Cmd + ";
    }
    if (m_modifiers & Modifier::Shift) {
        res += "Shift + ";
    }
    if (m_modifiers & Modifier::Alt) {
        res += "Alt + ";
    }
    res += mouseToString(m_button);
    return res;
}
DeviceID MouseBind::getDeviceID() const {
    return "mouse"_spr;
}
matjson::Value MouseBind::save() const {
    return matjson::Object {
        { "button", static_cast<int>(m_button) },
        { "modifiers", static_cast<int>(m_modifiers) },
    };
}
