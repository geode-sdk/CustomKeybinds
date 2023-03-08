#include "../include/Keybinds.hpp"

USE_GEODE_NAMESPACE();
using namespace keybinds;

Modifier keybinds::operator|(Modifier const& a, Modifier const& b) {
    return static_cast<Modifier>(static_cast<int>(a) | static_cast<int>(b));
}

Modifier keybinds::operator&(Modifier const& a, Modifier const& b) {
    return static_cast<Modifier>(static_cast<int>(a) & static_cast<int>(b));
}

bool Keybind::operator==(Keybind const& other) const {
    return key == other.key && modifiers == other.modifiers;
}

std::string Keybind::toString() const {
}

std::string KeybindAction::getID() const {
    return m_id;
}

std::vector<Keybind> KeybindAction::getDefaults() const {
    return m_defaults;
}

KeybindAction::KeybindAction(
    std::string const& id,
    std::vector<Keybind> const& defaults
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
