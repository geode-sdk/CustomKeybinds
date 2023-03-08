#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/utils/MiniFunction.hpp>
#include <Geode/loader/Event.hpp>
#include <cocos2d.h>

#ifdef GEODE_IS_WINDOWS
    #ifdef HJFOD_CUSTOM_KEYBINDS_EXPORTING
        #define CUSTOM_KEYBINDS_DLL __declspec(dllexport)
    #else
        #define CUSTOM_KEYBINDS_DLL __declspec(dllimport)
    #endif
#else
    #define CUSTOM_KEYBINDS_DLL
#endif

namespace keybinds {
    enum class Modifier : int {
        None        = 0b000,
        Control     = 0b001,
        Shift       = 0b010,
        Alt         = 0b100,
    };
    CUSTOM_KEYBINDS_DLL Modifier operator|(Modifier const& a, Modifier const& b);
    CUSTOM_KEYBINDS_DLL Modifier operator&(Modifier const& a, Modifier const& b);
    
    /**
     * Base class for implementing bindings for different input devices
     */
    struct CUSTOM_KEYBINDS_DLL Bind {
        virtual std::string toString() const = 0;
        virtual ~Bind() = default;
    };

    struct CUSTOM_KEYBINDS_DLL Keybind : public Bind {
        cocos2d::enumKeyCodes key;
        Modifier modifiers;

        bool operator==(Keybind const&) const;
        std::string toString() const override;
    };
}

namespace std {
    template<>
    struct hash<keybinds::Keybind> {
        CUSTOM_KEYBINDS_DLL std::size_t operator()(keybinds::Keybind const&) const;
    };
}

namespace keybinds {
    // i hate working with shared_from_this and inheritance so i'm just gonna 
    // use CCObject

    class CUSTOM_KEYBINDS_DLL KeybindAction : public cocos2d::CCObject {
    protected:
        std::string m_id;
        std::vector<Keybind> m_defaults;
    
    public:
        std::string getID() const;
        std::vector<Keybind> getDefaults() const;

        KeybindAction(std::string const& id, std::vector<Keybind> const& defaults);
    };

    class CUSTOM_KEYBINDS_DLL KeybindEvent : public geode::Event {
    protected:
        KeybindAction* m_action;

    public:
        KeybindEvent(KeybindAction* action);

        std::string getID() const;
    };

    class CUSTOM_KEYBINDS_DLL KeybindFilter : public geode::EventFilter<KeybindEvent> {
    protected:
        std::string m_id;

    public:
        using Callback = ListenerResult(KeybindEvent*);

        ListenerResult handle(geode::utils::MiniFunction<Callback> fn, KeybindEvent* event);
        KeybindFilter(std::string const& id);
    };

    class CUSTOM_KEYBINDS_DLL KeybindManager {
    protected:
        std::unordered_map<Keybind, std::vector<std::string>> m_keybinds;

    public:
        static KeybindManager* get();

        void addBindable(std::string const& id, std::vector<Keybind> const& defaults);

        void dispatch(std::string const& id);
    };
}
