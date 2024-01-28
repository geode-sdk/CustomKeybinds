#include "../include/Keybinds.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Event.hpp"
#include <Geode/utils/ranges.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/loader/ModEvent.hpp>
#include <GUI/CCControlExtension/CCScale9Sprite.h>

using namespace geode::prelude;
using namespace keybinds;

Modifier keybinds::operator|=(Modifier& a, Modifier const& b) {
    return static_cast<Modifier>(reinterpret_cast<int&>(a) |= static_cast<int>(b));
}

Modifier keybinds::operator|(Modifier const& a, Modifier const& b) {
    return static_cast<Modifier>(static_cast<int>(a) | static_cast<int>(b));
}

bool keybinds::operator&(Modifier const& a, Modifier const& b) {
    return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
}

std::string keybinds::keyToString(enumKeyCodes key) {
    switch (key) {
        case KEY_None:      return "";
        case KEY_C:         return "C";
        case KEY_Multiply:  return "Mul";
        case KEY_Divide:    return "Div";
        case KEY_OEMPlus:   return "Plus";
        case KEY_OEMMinus:  return "Minus";
        case CONTROLLER_LTHUMBSTICK_DOWN: return "L_THUMBSTICK_DOWN";
        case CONTROLLER_LTHUMBSTICK_LEFT: return "L_THUMBSTICK_LEFT";
        case CONTROLLER_LTHUMBSTICK_UP: return "L_THUMBSTICK_UP";
        case CONTROLLER_LTHUMBSTICK_RIGHT: return "L_THUMBSTICK_RIGHT";
        case CONTROLLER_RTHUMBSTICK_RIGHT: return "R_THUMBSTICK_RIGHT";
        case CONTROLLER_RTHUMBSTICK_LEFT: return "L_THUMBSTICK_RIGHT";
        case CONTROLLER_RTHUMBSTICK_DOWN: return "L_THUMBSTICK_DOWN";
        case CONTROLLER_RTHUMBSTICK_UP: return "L_THUMBSTICK_UP";
        case static_cast<enumKeyCodes>(-1): return "Unk";
        default: return CCKeyboardDispatcher::get()->keyToString(key);
    }
}

bool keybinds::keyIsModifier(enumKeyCodes key) {
    return
        key == KEY_Control ||
        key == KEY_LeftControl ||
        key == KEY_RightContol ||
        key == KEY_Shift ||
        key == KEY_LeftShift ||
        key == KEY_RightShift ||
        key == KEY_Alt ||
        key == KEY_LeftWindowsKey ||
        key == KEY_RightWindowsKey;
}

bool keybinds::keyIsController(enumKeyCodes key) {
    return key >= CONTROLLER_A && key <= CONTROLLER_RTHUMBSTICK_RIGHT;
}

bool Bind::isEqual(Bind* other) const {
    return this->getHash() == other->getHash();
}

CCNode* Bind::createLabel() const {
    return CCLabelBMFont::create(this->toString().c_str(), "goldFont.fnt");
}

CCNodeRGBA* Bind::createBindSprite() const {
    auto bg = CCScale9Sprite::create("square.png"_spr);
    bg->setOpacity(85);
    bg->setColor({ 0, 0, 0 });
    bg->setScale(.45f);

    auto top = this->createLabel();
    limitNodeSize(top, { 125.f, 30.f }, 1.f, .1f);
    bg->setContentSize({
        clamp(top->getScaledContentSize().width + 18.f, 18.f / bg->getScale(), 145.f),
        18.f / bg->getScale()
    });
    bg->addChild(top);

    top->setPosition(bg->getContentSize() / 2);

    return bg;
}

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
        static_cast<enumKeyCodes>(value["button"].as_double())
    );
}

matjson::Value ControllerBind::save() const {
    return matjson::Object {
        { "button", static_cast<int>(m_button) },
    };
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

BindHash::BindHash(Bind* bind) : bind(bind) {}

bool BindHash::operator==(BindHash const& other) const {
    return bind->isEqual(other.bind);
}

std::size_t std::hash<keybinds::BindHash>::operator()(keybinds::BindHash const& hash) const {
    return hash.bind ? (hash.bind->getHash() | std::hash<std::string>()(hash.bind->getDeviceID())) : 0;
}

Category::Category(const char* path) : m_value(path) {}

Category::Category(std::string const& path) : m_value(path) {}

std::vector<std::string> Category::getPath() const {
    return string::split(m_value, "/");
}

std::optional<Category> Category::getParent() const {
    if (string::contains(m_value, '/')) {
        return Category(m_value.substr(0, m_value.find_last_of('/')));
    }
    return std::nullopt;
}

bool Category::hasParent(Category const& parent) const {
    return m_value.starts_with(parent.m_value);
}

std::string Category::toString() const {
    return m_value;
}

bool Category::operator==(Category const& other) const {
    return m_value == other.m_value;
}

std::string BindableAction::getID() const {
    return m_id;
}

std::string BindableAction::getName() const {
    return m_name.empty() ? m_id : m_name;
}

std::string BindableAction::getDescription() const {
    return m_description;
}

Mod* BindableAction::getMod() const {
    return m_owner;
}

std::vector<Ref<Bind>> BindableAction::getDefaults() const {
    return m_defaults;
}

Category BindableAction::getCategory() const {
    return m_category;
}

bool BindableAction::isRepeatable() const {
    return m_repeatable;
}

BindableAction::BindableAction(
    ActionID const& id,
    std::string const& name,
    std::string const& description,
    std::vector<Ref<Bind>> const& defaults,
    Category const& category,
    bool repeatable,
    Mod* owner
) : m_id(id),
    m_owner(owner),
    m_name(name),
    m_description(description),
    m_category(category),
    m_repeatable(repeatable),
    m_defaults(defaults) {}

InvokeBindEvent::InvokeBindEvent(ActionID const& id, bool down) : m_id(id), m_down(down) {}

std::string InvokeBindEvent::getID() const {
    return m_id;
}

bool InvokeBindEvent::isDown() const {
    return m_down;
}

ListenerResult InvokeBindFilter::handle(utils::MiniFunction<Callback> fn, InvokeBindEvent* event) {
    if (event->getID() == m_id) {
        return fn(event);
    }
    return ListenerResult::Propagate;
}

InvokeBindFilter::InvokeBindFilter(CCNode* target, ActionID const& id)
  : m_target(target), m_id(id) {
    BindManager::get()->stopAllRepeats();
}

PressBindEvent::PressBindEvent(Bind* bind, bool down) : m_bind(bind), m_down(down) {}

Bind* PressBindEvent::getBind() const {
    return m_bind;
}

bool PressBindEvent::isDown() const {
    return m_down;
}

geode::ListenerResult PressBindFilter::handle(MiniFunction<Callback> fn, PressBindEvent* event) {
    return fn(event);
}

PressBindFilter::PressBindFilter() {}

DeviceEvent::DeviceEvent(DeviceID const& id, bool attached)
  : m_id(id), m_attached(attached) {}

DeviceID DeviceEvent::getID() const {
    return m_id;
}

bool DeviceEvent::wasAttached() const {
    return m_attached;
}

bool DeviceEvent::wasDetached() const {
    return !m_attached;
}

ListenerResult DeviceFilter::handle(MiniFunction<Callback> fn, DeviceEvent* event) {
    if (!m_id || m_id == event->getID()) {
        fn(event);
    }
    return ListenerResult::Propagate;
}

DeviceFilter::DeviceFilter(std::optional<DeviceID> id) : m_id(id) {}

BindManager::BindManager() {
    this->addCategory(Category::GLOBAL);
    this->addCategory(Category::PLAY);
    this->addCategory(Category::EDITOR);
    this->attachDevice("keyboard"_spr, &Keybind::parse);
    this->retain();
}

BindManager* BindManager::get() {
    static auto inst = new BindManager();
    return inst;
}

void BindManager::attachDevice(DeviceID const& device, BindParser parser) {
    if (m_devices.contains(device)) return;
    m_devices.insert({ device, parser });
    for (auto& [action, binds] : m_devicelessBinds[device]) {
        for (auto& data : binds) {
            // parser may fail
            try {
                if (auto nbind = parser(data)) {
                    this->addBindTo(action, nbind);
                }
            }
            catch(...) {}
        }
    }
    m_devicelessBinds.erase(device);
    DeviceEvent(device, true).post();
}

void BindManager::detachDevice(DeviceID const& device) {
    // Remove all binds related to this device from actions
    // The purpose of this is so they don't show up in the UI and can't be
    // modified, since the attached device isn't inserted so modifying them
    // wouldn't be possible anyway and they shouldn't get accidentally reset
    // if other binds are reset
    for (auto& [bind, actions] : m_binds) {
        if (bind.bind->getDeviceID() != device) {
            continue;
        }
        for (auto& action : actions) {
            // Bind::save may fail
            try {
                m_devicelessBinds[device][action].insert(this->saveBind(bind.bind));
            }
            catch(...) {}
            this->removeBindFrom(action, bind.bind);
        }
    }
    m_devices.erase(device);
    DeviceEvent(device, false).post();
}

matjson::Value BindManager::saveBind(Bind* bind) const {
    try {
        auto json = bind->save();
        json["device"] = bind->getDeviceID();
        return json;
    }
    catch(...) {
        return {};
    }
}

Bind* BindManager::loadBind(matjson::Value const& json) const {
    try {
        auto device = json["device"].as_string();
        if (!m_devices.contains(device)) {
            return nullptr;
        }
        return m_devices.at(device)(json);
    }
    catch(...) {
        return nullptr;
    }
}

bool BindManager::loadActionBinds(ActionID const& action) {
    try {
        auto value = Mod::get()->template getSavedValue<matjson::Object>(action);
        for (auto bind : value["binds"].as_array()) {
            // try directly parsing the bind from a string if the device it's for
            // is already connected
            if (auto b = this->loadBind(bind)) {
                this->addBindTo(action, b);
            }
            // otherwise save the bind's data for until the device is connected
            // or the game is closed
            else {
                // if device ID exists, then add this to the list of unbound
                // binds
                if (bind.contains("device")) {
                    try {
                        m_devicelessBinds[bind["device"].as_string()][action].insert(bind);
                    }
                    catch(...) {}
                }
                // otherwise invalid bind save data
            }
        }
        // load repeat options
        if (value.contains("repeat")) {
            auto rep = value["repeat"].as_object();
            auto opts = RepeatOptions();
            opts.enabled = rep["enabled"].as_bool();
            opts.rate = rep["rate"].as_int();
            opts.delay = rep["delay"].as_int();
            this->setRepeatOptionsFor(action, opts);
        }
        return true;
    }
    catch(...) {
        return false;
    }
}

void BindManager::saveActionBinds(ActionID const& action) {
    auto obj = matjson::Object();
    auto binds = matjson::Array();
    for (auto& bind : this->getBindsFor(action)) {
        binds.push_back(this->saveBind(bind));
    }
    for (auto& [device, actions] : m_devicelessBinds) {
        if (actions.contains(action)) {
            for (auto& bind : actions.at(action)) {
                binds.push_back(bind);
            }
        }
    }
    obj["binds"] = binds;
    if (auto opts = this->getRepeatOptionsFor(action)) {
        auto rep = matjson::Object();
        rep["enabled"] = opts.value().enabled;
        rep["rate"] = opts.value().rate;
        rep["delay"] = opts.value().delay;
        obj["repeat"] = rep;
    }
    Mod::get()->setSavedValue(action, obj);
}

bool BindManager::registerBindable(BindableAction const& action, ActionID const& after) {
    this->stopAllRepeats();
    if (ranges::contains(m_actions, [&](auto const& act) { return act.first == action.getID(); })) {
        return false;
    }
    if (auto ix = ranges::indexOf(m_actions, [&](auto const& a) { return a.first == after; })) {
        m_actions.insert(m_actions.begin() + ix.value() + 1, {
            action.getID(),
            { .definition = action, .repeat = RepeatOptions() }
        });
    }
    else {
        m_actions.push_back({
            action.getID(),
            { .definition = action, .repeat = RepeatOptions() }
        });
    }
    this->addCategory(action.getCategory());
    if (!this->loadActionBinds(action.getID())) {
        for (auto& def : action.getDefaults()) {
            this->addBindTo(action.getID(), def);
        }
    }
    return true;
}

void BindManager::removeBindable(ActionID const& action) {
    this->stopAllRepeats();
    this->removeAllBindsFrom(action);
    ranges::remove(m_actions, [&](auto const& act) { return act.first == action; });
}

std::optional<BindableAction> BindManager::getBindable(ActionID const& action) const {
    for (auto& [id, bindable] : m_actions) {
        if (id == action) {
            return bindable.definition;
        }
    }
    return std::nullopt;
}

std::vector<BindableAction> BindManager::getAllBindables() const {
    std::vector<BindableAction> res;
    for (auto& [_, action] : m_actions) {
        res.push_back(action.definition);
    }
    return res;
}

std::vector<BindableAction> BindManager::getBindablesIn(Category const& category, bool sub) const {
    std::vector<BindableAction> res;
    for (auto& [_, action] : m_actions) {
        if (sub ?
            action.definition.getCategory().hasParent(category) :
            (action.definition.getCategory() == category)
        ) {
            res.push_back(action.definition);
        }
    }
    return res;
}

std::vector<BindableAction> BindManager::getBindablesFor(Bind* bind) const {
    std::vector<BindableAction> res {};
    if (m_binds.count(bind)) {
        for (auto& bindable : m_binds.at(bind)) {
            if (auto action = this->getBindable(bindable)) {
                res.push_back(action.value());
            }
        }
    }
    return res;
}

std::vector<Category> BindManager::getAllCategories() const {
    return m_categories;
}

void BindManager::addCategory(Category const& category) {
    this->stopAllRepeats();
    if (!ranges::contains(m_categories, category)) {
        // Add parent categories first if they don't exist yet
        if (auto parent = category.getParent()) {
            this->addCategory(parent.value());
        }
        auto it = m_categories.begin();
        bool foundParent = false;
        for (auto& cat : m_categories) {
            if (cat.hasParent(category)) {
                foundParent = true;
            }
            else if (foundParent) {
                break;
            }
            it++;
        }
        m_categories.insert(it, category);
    }
}

void BindManager::removeCategory(Category const& category) {
    this->stopAllRepeats();
    for (auto& bindable : this->getBindablesIn(category, true)) {
        this->removeBindable(bindable.getID());
    }
    ranges::remove(m_categories, [&](auto const& cat) { return cat.hasParent(category); });
}

void BindManager::addBindTo(ActionID const& action, Bind* bind) {
    this->stopAllRepeats();
    if (m_devices.contains(bind->getDeviceID())) {
        m_binds[bind].push_back(action);
    }
    else {
        m_devicelessBinds[bind->getDeviceID()][action].insert(this->saveBind(bind));
    }
}

void BindManager::removeBindFrom(ActionID const& action, Bind* bind) {
    this->stopAllRepeats();
    ranges::remove(m_binds[bind], action);
    if (m_binds.at(bind).empty()) {
        m_binds.erase(bind);
    }
}

void BindManager::removeAllBindsFrom(ActionID const& action) {
    this->stopAllRepeats();
    for (auto& [bind, actions] : m_binds) {
        ranges::remove(actions, action);
    }
}

std::vector<Ref<Bind>> BindManager::getBindsFor(ActionID const& action) const {
    std::vector<Ref<Bind>> binds;
    for (auto& [bind, actions] : m_binds) {
        if (ranges::contains(actions, action)) {
            binds.push_back(bind.bind);
        }
    }
    return binds;
}

void BindManager::resetBindsToDefault(ActionID const& action) {
    this->stopAllRepeats();
    this->removeAllBindsFrom(action);
    if (auto bindable = this->getBindable(action)) {
        for (auto& def : bindable->getDefaults()) {
            this->addBindTo(action, def);
        }
    }
}

bool BindManager::hasDefaultBinds(ActionID const& action) const {
    if (auto bindable = this->getBindable(action)) {
        auto binds = this->getBindsFor(action);
        auto defs = bindable->getDefaults();
        ranges::remove(defs, [=](auto const& def) {
            return !m_devices.contains(def->getDeviceID());
        });
        if (binds.size() == defs.size()) {
            for (size_t i = 0; i < binds.size(); i++) {
                if (!binds.at(i)->isEqual(defs.at(i))) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

std::optional<RepeatOptions> BindManager::getRepeatOptionsFor(ActionID const& action) {
    for (auto& [id, bindable] : m_actions) {
        if (id == action) {
            if (bindable.definition.isRepeatable()) {
                return bindable.repeat;
            }
        }
    }
    return std::nullopt;
}

void BindManager::setRepeatOptionsFor(ActionID const& action, RepeatOptions const& options) {
    this->stopAllRepeats();
    for (auto& [id, bindable] : m_actions) {
        if (id == action) {
            bindable.repeat = options;
        }
    }
}

ListenerResult BindManager::onDispatch(PressBindEvent* event) {
    if (m_binds.contains(event->getBind())) {
        for (auto& action : m_binds.at(event->getBind())) {
            bool inserted = false;
            if (event->isDown()) {
                if (!m_held.contains(action)) {
                    m_held.insert(action);
                    inserted = true;
                }
                if (auto options = this->getRepeatOptionsFor(action)) {
                    if (options.value().enabled && ranges::contains(m_repeating, [=](auto const& p) { return p.first == action; })) {
                        return ListenerResult::Stop;
                    }
                }
                this->repeat(action);
            }
            else {
                m_held.erase(action);
                this->unrepeat(action);
            }
            auto options = this->getRepeatOptionsFor(action);
            if ((!options.has_value() || !options.value().enabled) && !inserted && m_held.contains(action)) {
                return ListenerResult::Stop;
            }
            if (InvokeBindEvent(action, event->isDown()).post() == ListenerResult::Stop) {
                return ListenerResult::Stop;
            }
        }
    }
    return ListenerResult::Propagate;
}

void BindManager::stopAllRepeats() {
    m_repeating.clear();
    CCScheduler::get()->unscheduleSelector(
        schedule_selector(BindManager::onRepeat), this
    );
}

void BindManager::unrepeat(ActionID const& action) {
    ranges::remove(m_repeating, [=](auto const& p) { return p.first == action; });
}

void BindManager::repeat(ActionID const& action) {
    // this is why you are supposed to pimpl stuff now i cant change the return type
    if (auto options = this->getRepeatOptionsFor(action)) {
        if (options.value().enabled) {
            m_repeating.emplace_back(action, options.value().delay / 1000.f);
            CCScheduler::get()->scheduleSelector(
                schedule_selector(BindManager::onRepeat), this,
                0.f, false
            );
        }
    }
}

void BindManager::onRepeat(float dt) {
    for (auto& [id, last] : m_repeating) {
        if (auto options = this->getRepeatOptionsFor(id)) {
            last -= dt;
            if (last < 0.f) {
                InvokeBindEvent(id, true).post();
                last += options.value().rate / 1000.f;
            }
        }
    }
}

void BindManager::save() {
    for (auto& bindable : BindManager::get()->getAllBindables()) {
        BindManager::get()->saveActionBinds(bindable.getID());
    }
}

$on_mod(DataSaved) {
    BindManager::get()->save();
}
