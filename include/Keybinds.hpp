#pragma once

#include <Geode/DefaultInclude.hpp>
#include <Geode/utils/MiniFunction.hpp>
#include <Geode/utils/cocos.hpp>
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
    class CUSTOM_KEYBINDS_DLL Bind : public cocos2d::CCObject {
    public:
        virtual size_t getHash() const = 0;
        virtual bool isEqual(Bind* other) const = 0;
        virtual std::string toString() const = 0;
        virtual ~Bind() = default;
    };

    class CUSTOM_KEYBINDS_DLL Keybind : public Bind {
    protected:
        cocos2d::enumKeyCodes m_key;
        Modifier m_modifiers;

    public:
        static Keybind* create(cocos2d::enumKeyCodes key, Modifier modifiers);

        cocos2d::enumKeyCodes getKey() const;
        Modifier getModifiers() const;

        size_t getHash() const override;
        bool isEqual(Bind* other) const override;
        std::string toString() const override;
    };

    struct CUSTOM_KEYBINDS_DLL BindHash {
        geode::Ref<Bind> bind;
        bool operator==(BindHash const& other) const;
    };
}

namespace std {
    template<>
    struct hash<keybinds::BindHash> {
        CUSTOM_KEYBINDS_DLL std::size_t operator()(keybinds::BindHash const&) const;
    };
}

namespace keybinds {
    // i hate working with shared_from_this and inheritance so i'm just gonna 
    // use CCObject

    class CUSTOM_KEYBINDS_DLL BindableAction : public cocos2d::CCObject {
    protected:
        std::string m_id;
        std::vector<geode::Ref<Bind>> m_defaults;
    
    public:
        std::string getID() const;
        std::vector<geode::Ref<Bind>> getDefaults() const;

        BindableAction(std::string const& id, std::vector<geode::Ref<Bind>> const& defaults);
    };

    class CUSTOM_KEYBINDS_DLL BindEvent : public geode::Event {
    protected:
        BindableAction* m_action;

    public:
        BindEvent(BindableAction* action);

        std::string getID() const;
    };

    class CUSTOM_KEYBINDS_DLL BindFilter : public geode::EventFilter<BindEvent> {
    protected:
        std::string m_id;

    public:
        using Callback = ListenerResult(BindEvent*);

        ListenerResult handle(geode::utils::MiniFunction<Callback> fn, BindEvent* event);
        BindFilter(std::string const& id);
    };

    class CUSTOM_KEYBINDS_DLL KeybindManager {
    protected:
        std::unordered_map<BindHash, std::vector<std::string>> m_keybinds;

    public:
        static KeybindManager* get();

        void addBindable(std::string const& id, std::vector<Keybind> const& defaults);

        void dispatch(std::string const& id);
        void dispatch(Bind* bind);
    };
}
