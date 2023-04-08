#include "../include/Keybinds.hpp"

USE_GEODE_NAMESPACE();
using namespace keybinds;

Modifier keybinds::operator|(Modifier const& a, Modifier const& b) {
    return static_cast<Modifier>(static_cast<int>(a) | static_cast<int>(b));
}

Modifier keybinds::operator&(Modifier const& a, Modifier const& b) {
    return static_cast<Modifier>(static_cast<int>(a) & static_cast<int>(b));
}

Keybind* Keybind::create(cocos2d::enumKeyCodes key, Modifier modifiers) {
    auto ret = new Keybind();
    ret->m_key = key;
    ret->m_modifiers = modifiers;
    ret->autorelease();
    return ret;
}

cocos2d::enumKeyCodes Keybind::getKey() const {
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
}

bool BindHash::operator==(BindHash const& other) const {
    return bind->isEqual(other.bind);
}

std::size_t std::hash<keybinds::BindHash>::operator()(keybinds::BindHash const& hash) const {
    return hash.bind->getHash() | typeid(*hash.bind).hash_code();
}

std::string BindableAction::getID() const {
    return m_id;
}

std::vector<geode::Ref<Bind>> BindableAction::getDefaults() const {
    return m_defaults;
}

BindableAction::BindableAction(
    std::string const& id,
    std::vector<geode::Ref<Bind>> const& defaults
) : m_id(id), m_defaults(defaults) {}

KeybindEvent::KeybindEvent(KeybindAction* action) : m_action(action) {}

std::string KeybindEvent::getID() const {
    return m_action->getID();
}

ListenerResult KeybindFilter::handle(geode::utils::MiniFunction<Callback> fn, KeybindEvent* event) {
    if (event->getID() == m_id) {
        return fn(event);
    }
    return ListenerResult::Propagate;
}

KeybindFilter::KeybindFilter(std::string const& id) : m_id(id) {}
