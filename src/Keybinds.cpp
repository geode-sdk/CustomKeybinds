#include "../include/Keybinds.hpp"
#include <Geode/utils/ranges.hpp>
#include <Geode/utils/string.hpp>

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
        case static_cast<enumKeyCodes>(-1): return "Unk";
        default: return CCKeyboardDispatcher::get()->keyToString(key);
    }
}

bool keybinds::keyIsModifier(cocos2d::enumKeyCodes key) {
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

static std::unordered_map<std::string, Bind::DataLoader> INPUT_DEVICES {};

void Bind::registerInputDevice(std::string const& id, Bind::DataLoader dataLoader) {
    INPUT_DEVICES.insert({ id, dataLoader });
}

Bind* Bind::parse(std::string const& data) {
    auto off = data.find_first_of(':');
    if (off == std::string::npos) {
        return nullptr;
    }
    auto id = data.substr(0, off);
    if (!INPUT_DEVICES.contains(id)) {
        return nullptr;
    }
    return INPUT_DEVICES.at(id)(data.substr(off + 1));
}

std::string Bind::save() const {
    // todo
    return "";
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

std::string Keybind::toSaveData() const {
    return std::to_string(static_cast<int>(m_modifiers)) + "|" + std::to_string(m_key);
}

BindHash::BindHash(Bind* bind) : bind(bind) {}

bool BindHash::operator==(BindHash const& other) const {
    return bind->isEqual(other.bind);
}

std::size_t std::hash<keybinds::BindHash>::operator()(keybinds::BindHash const& hash) const {
    return hash.bind ? (hash.bind->getHash() | typeid(*hash.bind).hash_code()) : 0;
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

geode::ListenerResult PressBindFilter::handle(geode::utils::MiniFunction<Callback> fn, PressBindEvent* event) {
    return fn(event);
}

PressBindFilter::PressBindFilter() {}

BindManager::BindManager() {
    this->addCategory(Category::GLOBAL);
    this->addCategory(Category::PLAY);
    this->addCategory(Category::EDITOR);
    this->retain();
}

BindManager* BindManager::get() {
    static auto inst = new BindManager();
    return inst;
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
    for (auto& def : action.getDefaults()) {
        this->addBindTo(action.getID(), def);
    }
    this->addCategory(action.getCategory());
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

std::vector<BindableAction> BindManager::getBindables(Category const& category, bool sub) const {
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
    for (auto& bindable : this->getBindables(category, true)) {
        this->removeBindable(bindable.getID());
    }
    ranges::remove(m_categories, [&](auto const& cat) { return cat.hasParent(category); });
}

void BindManager::addBindTo(ActionID const& action, Bind* bind) {
    this->stopAllRepeats();
    m_binds[bind].push_back(action);
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
            if (event->isDown()) {
                this->repeat(action);
            }
            else {
                this->unrepeat(action);
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

$on_mod(DataSaved) {
    auto m = BindManager::get();
    for (auto& bindable : m->getAllBindables()) {
        auto obj = json::Object();
        for (auto& bind : m->getBindsFor(bindable.getID())) {
            
        }
        if (auto options = m->getRepeatOptionsFor(bindable.getID())) {
            obj["repeat-enabled"] = options.value().enabled;
            obj["repeat-rate"] = options.value().rate;
            obj["repeat-delay"] = options.value().delay;
        }
    }
}
