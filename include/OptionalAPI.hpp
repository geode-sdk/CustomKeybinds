#pragma once

#include "Keybinds.hpp"
#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
    #undef MY_MOD_ID
#endif
#define MY_MOD_ID "geode.custom-keybinds"

namespace keybinds {
    enum class Modifier2 : unsigned int {
        None        = 0b0000,
        Control     = 0b0001,
        Shift       = 0b0010,
        Alt         = 0b0100,
        Command     = 0b1000,
    };

    inline Modifier2 operator|(Modifier2 const& a, Modifier2 const& b) {
        return static_cast<Modifier2>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
    }

    inline Modifier2& operator|=(Modifier2& a, Modifier2 const& b) {
        a = a | b;
        return a;
    }

    inline bool operator&(Modifier2 const& a, Modifier2 const& b) {
        return (static_cast<unsigned int>(a) & static_cast<unsigned int>(b)) != 0;
    }

    // TODO: move actual classes into these in v2
    class KeybindV2 final {
        class Impl;
        std::unique_ptr<Impl> m_impl;
    public:
        static geode::Result<Keybind*> create(cocos2d::enumKeyCodes key, Modifier modifiers = Modifier::None) GEODE_EVENT_EXPORT(&KeybindV2::create, (key, modifiers));
    };

    class MouseBindV2 final {
        class Impl;
        std::unique_ptr<Impl> m_impl;
    public:
        static geode::Result<MouseBind*> create(MouseButton button, Modifier modifiers = Modifier::None) GEODE_EVENT_EXPORT(&MouseBindV2::create, (button, modifiers));
    };

    class ControllerBindV2 final {
        class Impl;
        std::unique_ptr<Impl> m_impl;
    public:
        static geode::Result<ControllerBind*> create(cocos2d::enumKeyCodes button) GEODE_EVENT_EXPORT(&ControllerBindV2::create, (button));
    };

    
    class CategoryV2 final {
        class Impl;
        std::unique_ptr<Impl> m_impl;
    public:
        static geode::Result<std::shared_ptr<Category>> create(std::string_view path) GEODE_EVENT_EXPORT(&CategoryV2::create, (path));

        static inline auto const PLAY = CategoryV2::create("Play");
        static inline auto const PLAY_PAUSE = CategoryV2::create("Play/Pause");
        static inline auto const EDITOR = CategoryV2::create("Editor");
        static inline auto const GLOBAL = CategoryV2::create("Global");
        static inline auto const EDITOR_UI = CategoryV2::create("Editor/UI");
        static inline auto const EDITOR_MODIFY = CategoryV2::create("Editor/Modify");
        static inline auto const EDITOR_MOVE = CategoryV2::create("Editor/Move");
    };

    class BindableActionV2 final {
        class Impl;
        std::unique_ptr<Impl> m_impl;
    public:
        static geode::Result<std::shared_ptr<BindableAction>> create(
            ActionID const& id,
            std::string const& name,
            std::string const& description = "",
            std::vector<geode::Ref<Bind>> const& defaults = {},
            std::shared_ptr<Category> category = {},
            bool repeatable = true,
            geode::Mod* owner = geode::Mod::get()
        ) GEODE_EVENT_EXPORT(&BindableActionV2::create, (id, name, description, defaults, category, repeatable, owner));
    };
 
    class InvokeBindEventV2 final : public geode::Event {
    private:
        ActionID m_id;
        bool m_down;

        friend class BindManager;
        friend class InvokeBindFilterV2;

    public:
        InvokeBindEventV2(ActionID const& id, bool down) : m_id(id), m_down(down) {}
        ActionID getID() const {
            return m_id;    
        }
        bool isDown() const {
            return m_down;
        }
    };

    class InvokeBindFilterV2 final : public geode::EventFilter<InvokeBindEventV2> {
    private:
        cocos2d::CCNode* m_target;
        ActionID m_id;

    public:
        using Callback = geode::ListenerResult(InvokeBindEventV2*);

        geode::ListenerResult handle(std::function<Callback> fn, InvokeBindEventV2* event) {
            if (event->getID() == m_id) {
                return fn(event);
            }
            return geode::ListenerResult::Propagate;
        }
        InvokeBindFilterV2(cocos2d::CCNode* target, ActionID const& id) : m_target(target), m_id(id) {}
    };

    class BindManagerV2 {
        class Impl;
        std::unique_ptr<Impl> m_impl;
    public:
        static geode::Result<bool> registerBindable(
            std::shared_ptr<BindableAction> action,
            ActionID const& after = ""
        ) GEODE_EVENT_EXPORT(&BindManagerV2::registerBindable, (action, after));
        static geode::Result<> removeBindable(ActionID const& action) GEODE_EVENT_EXPORT(&BindManagerV2::removeBindable, (action));
        static geode::Result<std::shared_ptr<BindableAction>> getBindable(ActionID const& action) GEODE_EVENT_EXPORT(&BindManagerV2::getBindable, (action));
        static geode::Result<std::vector<std::shared_ptr<BindableAction>>> getAllBindables() GEODE_EVENT_EXPORT(&BindManagerV2::getAllBindables, ());
        static geode::Result<std::vector<std::shared_ptr<BindableAction>>> getBindablesIn(std::shared_ptr<Category> category, bool sub = false) GEODE_EVENT_EXPORT(&BindManagerV2::getBindablesIn, (category, sub));
        static geode::Result<std::vector<std::shared_ptr<BindableAction>>> getBindablesFor(Bind* bind) GEODE_EVENT_EXPORT(&BindManagerV2::getBindablesFor, (bind));

        static geode::Result<std::vector<std::shared_ptr<Category>>> getAllCategories() GEODE_EVENT_EXPORT(&BindManagerV2::getAllCategories, ());
        static geode::Result<> addCategory(std::shared_ptr<Category> category) GEODE_EVENT_EXPORT(&BindManagerV2::addCategory, (category));
        static geode::Result<> removeCategory(std::shared_ptr<Category> category) GEODE_EVENT_EXPORT(&BindManagerV2::removeCategory, (category));

        static geode::Result<std::vector<geode::Ref<Bind>>> getBindsFor(ActionID const& action) GEODE_EVENT_EXPORT(&BindManagerV2::getBindsFor, (action));
    };
}