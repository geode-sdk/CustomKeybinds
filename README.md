# Custom Keybinds

Adds support for customizing keybinds in the editor and while playing.

## Adding keybinds through other mods

You can use Custom Keybinds as a dependency by declaring it in your `mod.json`:

```jsonc
{
    "dependencies": {
        "geode.custom-keybinds": {
            "id": "geode.custom-keybinds",
            "version": "v1.1.0",
            "importance": "required"
        },
        // or the optional api version
        "geode.custom-keybinds": {
            "version": "v1.1.0",
            "importance": "recommended"
        }
    }
}
```

Adding a new keybind requires two things: registering it when your mod starts up, and handling callbacks when it's fired.

Registering can be done easily using `$execute`:

```cpp
#include <geode.custom-keybinds/include/Keybinds.hpp>
// optional api version
#include <geode.custom-keybinds/include/OptionalAPI.hpp>

using namespace keybinds;

$execute {
    BindManager::get()->registerBindable({
        // ID, should be prefixed with mod ID
        "backflip"_spr,
        // Name
        "Do a Backflip!",
        // Description, leave empty for none
        "Throw a backflip",
        // Default binds
        { Keybind::create(KEY_Q, Modifier::None) },
        // Category; use slashes for specifying subcategories. See the
        // Category class for default categories
        "My Mod/Awesome Tricks"
    });

    // optional api version
    (void)[&]() -> Result<> {
        GEODE_UNWRAP(BindManagerV2::registerBindable(GEODE_UNWRAP(BindableActionV2::create(
            // ID, should be prefixed with mod ID
            "backflip"_spr,
            // Name
            "Do a Backflip!",
            // Description, leave empty for none
            "Throw a backflip",
            // Default binds
            { GEODE_UNWRAP(KeybindV2::create(KEY_Q, ModifierV2::None)) },
            // Category; use slashes for specifying subcategories. See the
            // Category class for default categories
            GEODE_UNWRAP(CategoryV2::create("My Mod/Awesome Tricks"))
        ))));
        return Ok();
    }();
}
```

Now your bind shows up in the UI, and the user can assign their own binds to it. To make the bind also do stuff, you need to add an event listener for `InvokeBindEvent`. If your bind is global (can be used from anywhere in-game), you should add the event listener in the same `$execute` block. If the bind is layer-specific, add it in the layer's `init`:

```cpp
#include <geode.custom-keybinds/include/Keybinds.hpp>
// optional api version
#include <geode.custom-keybinds/include/OptionalAPI.hpp>

using namespace keybinds;

bool MyLayer::init() {
    ...

    this->addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
        if (event->isDown()) {
            // do a backflip!
        }
        // Return Propagate if you want other actions with the same bind to
        // also be fired, or Stop if you want to halt propagation
        return ListenerResult::Propagate;
    }, "backflip"_spr);

    // optional api version
    this->addEventListener<InvokeBindFilterV2>([this](InvokeBindEventV2* event) {
        if (event->isDown()) {
            // do a backflip!
        }
        // Return Propagate if you want other actions with the same bind to
        // also be fired, or Stop if you want to halt propagation
        return ListenerResult::Propagate;
    }, "backflip"_spr);

    ...
}
```

Adding keybinds to the editor / `PlayLayer` is the same - just add the keybind to the Editor / Play categories, and hook `EditorUI::init` or `UILayer::init` to handle the callback. You can also pass the ID of an existing action to `BindManager::registerBindable` to place the bind after, if you for example add a new build category and would like it after the `Delete Mode` bind.

## Global keybinds

You can listen for global keybinds via an `$execute` block:

```cpp
#include <geode.custom-keybinds/include/Keybinds.hpp>
// optional api version
#include <geode.custom-keybinds/include/OptionalAPI.hpp>

using namespace keybinds;

$execute {
    new EventListener(+[](InvokeBindEvent* event) {
        // do stuff
        return ListenerResult::Propagate;
    }, InvokeBindFilter(nullptr, "event-id"));

    // optional api version
    new EventListener(+[](InvokeBindEventV2* event) {
        // do stuff
        return ListenerResult::Propagate;
    }, InvokeBindFilterV2(nullptr, "event-id"));
}
```

## Programmatically triggering binds

You can invoke a bind by creating and posting an `InvokeBindEvent`:

```cpp
InvokeBindEvent("backflip"_spr, true).post();
```

You can also trigger a specific key combination with `PressBindEvent`:

```cpp
PressBindEvent(Keybind::create(KEY_X, Modifier::None)).post();
```

## Adding new input devices

Custom keybinds has been written to support more input devices, in case you want to add first-class support for a gaming toaster to GD.

To add a new input device, first thing you should do is create a new bind class:

```cpp
#include <geode.custom-keybinds/include/Keybinds.hpp>

using namespace keybinds;

class ToasterBind : public Bind {
public:
    BreadType m_bread;
    float m_temperature;

public:
    static ToasterBind* create(BreadType type, float temperature) {
        auto ret = new ToasterBind();
        ret->autorelease();
        ret->m_bread = type;
        ret->m_temperature = temperature;
        return ret;
    }
    // Parse from JSON
    static ToasterBind* parse(matjson::Value const& json) {
        return ToasterBind::create(
            static_cast<BreadType>(json["bread"].as_int()),
            static_cast<float>(json["temperature"].as_double())
        );
    }
    // Save to JSON
    matjson::Value save() const override {
        return matjson::Object {
            { "bread", static_cast<int>(m_bread) },
            { "temperature", m_temperature },
        };
    }

    // Getters for members
    BreadType getThisBread() const;
    float getTemperature() const;

    // Get the hash for this bind
    size_t getHash() const override {
        return std::hash<float>()(m_temperature) ^ static_cast<size_t>(m_bread);
    }

    // Check if this bind is equal to another
    bool isEqual(Bind* other) const override {
        if (auto o = typeinfo_cast<ToasterBind*>(other)) {
            return m_temperature == o->m_temperature && m_bread == o->m_bread;
        }
        return false;
    }

    // Convert to string
    // By default, the bind is displayed in the UI as just the string in a
    // label. If you want to show something else, override createLabel()
    std::string toString() const override {
        return fmt::format("Temp {} & Bread {}", m_temperature, m_bread);
    }

    // The device this bind is related to
    DeviceID getDeviceID() const override {
        return "toaster"_spr;
    }
};
```

Registering a new input device is as simple as calling `BindManager::attachDevice`:

```cpp
// Device ID should be prefixed with mod ID as usual
BindManager::get()->attachDevice("toaster"_spr, &MyParser::parse);
```

If the device is detached while the game is running, you can unregister it similarly with `BindManager::detachDevice`.

Once you have figured out how to get inputs from your awesome toaster, you need to post `PressBindEvent`s:

```cpp
void XInput_onGamingToasterCallback(double temp, int bread) {
    // Make sure all bind events are only ever posted in the GD thread !!!!
	Loader::get()->queueInMainThread([temp, bread] {
        PressBindEvent(ToasterBind::create(static_cast<float>(temp), bread), true).post();
    });
}
```
