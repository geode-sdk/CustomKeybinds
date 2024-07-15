#include "../include/Keybinds.hpp"

using namespace geode::prelude;
using namespace keybinds;

Keybind* Keybind::create(enumKeyCodes key, Modifier modifiers) {
    if (key == KEY_None || key == KEY_Unknown || keyIsController(key)) {
        return nullptr;
    }
    auto ret = new Keybind();
    ret->m_key = key;
    ret->m_modifiers = modifiers;
    ret->autorelease();
    return ret;
}

Keybind* Keybind::parse(matjson::Value const& value) {
    return Keybind::create(
        static_cast<enumKeyCodes>(value["key"].as_int()),
        static_cast<Modifier>(value["modifiers"].as_int())
    );
}

matjson::Value Keybind::save() const {
    return matjson::Object {
        { "key", static_cast<int>(m_key) },
        { "modifiers", static_cast<int>(m_modifiers) }
    };
}

enumKeyCodes Keybind::getKey() const {
    return m_key;
}

Modifier Keybind::getModifiers() const {
    return m_modifiers;
}

size_t Keybind::getHash() const {
    return m_key | (static_cast<int>(m_modifiers) << 29);
}

bool Keybind::isEqual(Bind* other) const {
    if (auto o = typeinfo_cast<Keybind*>(other)) {
        return m_key == o->m_key && m_modifiers == o->m_modifiers;
    }
    return false;
}

std::string Keybind::toString() const {
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
    res += keyToString(m_key);
    return res;
}

std::string Keybind::getDeviceID() const {
    return "keyboard"_spr;
}
