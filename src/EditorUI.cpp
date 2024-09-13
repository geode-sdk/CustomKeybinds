#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <unordered_map>
#include <unordered_set>
#include "../include/Keybinds.hpp"
#include "Geode/Enums.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"

using namespace geode::prelude;
using namespace keybinds;

// TODO: move this to header in 1.2.0

class EvilBypass : public CCKeyboardDispatcher {
public:
    bool setControlPressed(bool pressed) {
        auto old = m_bControlPressed;
        m_bControlPressed = pressed;
        return old;
    }
    bool setCommandPressed(bool pressed) {
        auto old = m_bCommandPressed;
        m_bCommandPressed = pressed;
        return old;
    }
    bool setShiftPressed(bool pressed) {
        auto old = m_bShiftPressed;
        m_bShiftPressed = pressed;
        return old;
    }
    bool setAltPressed(bool pressed) {
        auto old = m_bAltPressed;
        m_bAltPressed = pressed;
        return old;
    }
};

struct $modify(EditorPauseLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorPauseLayer::keyDown", 1000);
    }

    void customSetup() {
        EditorPauseLayer::customSetup();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (event->isDown()) {
                this->onResume(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        }, "robtop.geometry-dash/unpause-level");
    }

    void keyDown(enumKeyCodes key) {
        if (key == enumKeyCodes::KEY_Escape) {
            EditorPauseLayer::keyDown(key);
        }
    }
};

struct $modify(EditorUI) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::keyDown", 1000000);
        (void)self.setHookPriority("EditorUI::keyUp", 1000);
    }

    static inline int platformButton() {
        return 1;
    }

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        Loader::get()->queueInMainThread([this, lel] {
            this->defineKeybind("robtop.geometry-dash/jump-p1", [lel](bool down) {
                if (lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(platformButton(), down, false);
                }
            });
            this->defineKeybind("robtop.geometry-dash/jump-p2", [lel](bool down) {
                if (lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(platformButton(), down, true);
                }
            });
            this->defineKeybind("robtop.geometry-dash/move-left-p1", [lel](bool down) {
                if (lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Left), down, false);
                }
            });
            this->defineKeybind("robtop.geometry-dash/move-right-p1", [lel](bool down) {
                if (lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Right), down, false);
                }
            });
            this->defineKeybind("robtop.geometry-dash/move-left-p2", [lel](bool down) {
                if (lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Left), down, true);
                }
            });
            this->defineKeybind("robtop.geometry-dash/move-right-p2", [lel](bool down) {
                if (lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Right), down, true);
                }
            });
            this->defineKeybind("robtop.geometry-dash/build-mode", [this] {
                this->passThroughKeyDown(KEY_One);
            });
            this->defineKeybind("robtop.geometry-dash/edit-mode", [this] {
                this->passThroughKeyDown(KEY_Two);
            });
            this->defineKeybind("robtop.geometry-dash/delete-mode", [this] {
                this->passThroughKeyDown(KEY_Three);
            });
            this->defineKeybind("robtop.geometry-dash/rotate-ccw", [this] {
                this->passThroughKeyDown(KEY_Q);
            });
            this->defineKeybind("robtop.geometry-dash/rotate-cw", [this] {
                this->passThroughKeyDown(KEY_E);
            });
            this->defineKeybind("robtop.geometry-dash/flip-x", [this] {
                this->passThroughKeyDown(KEY_Q, Modifier::Alt);
            });
            this->defineKeybind("robtop.geometry-dash/flip-y", [this] {
                this->passThroughKeyDown(KEY_E, Modifier::Alt);
            });
            this->defineKeybind("robtop.geometry-dash/delete", [this] {
                this->passThroughKeyDown(KEY_Delete);
            });
            this->defineKeybind("robtop.geometry-dash/undo", [this] {
                this->passThroughKeyDown(KEY_Z, Modifier::PlatformControl);
            });
            this->defineKeybind("robtop.geometry-dash/redo", [this] {
                this->passThroughKeyDown(KEY_Z, Modifier::PlatformControl | Modifier::Shift);
            });
            this->defineKeybind("robtop.geometry-dash/deselect-all", [this] {
                this->passThroughKeyDown(KEY_D, Modifier::Alt);
            });
            this->defineKeybind("robtop.geometry-dash/copy", [this] {
                this->passThroughKeyDown(KEY_C, Modifier::PlatformControl);
            });
            this->defineKeybind("robtop.geometry-dash/paste", [this] {
                this->passThroughKeyDown(KEY_V, Modifier::PlatformControl);
            });
            this->defineKeybind("robtop.geometry-dash/copy-paste", [this] {
                this->passThroughKeyDown(KEY_D, Modifier::PlatformControl);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-rotate", [this] {
                this->passThroughKeyDown(KEY_R);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-transform", [this] {
                this->passThroughKeyDown(KEY_T, Modifier::PlatformControl);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-free-move", [this] {
                this->passThroughKeyDown(KEY_F);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-swipe", [this] {
                this->passThroughKeyDown(KEY_T);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-snap", [this] {
                this->passThroughKeyDown(KEY_G);
            });
            this->defineKeybind("robtop.geometry-dash/playtest", [this] {
                this->passThroughKeyDown(KEY_Enter);
            });
            this->defineKeybind("robtop.geometry-dash/playback-music", [this] {
                // RobTop broke this in 2.2, which makes it trigger the playtest keybind
                // this->passThroughKeyDown(KEY_Enter, Modifier::PlatformControl);
                EditorUI::onPlayback(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/prev-build-tab", [this] {
                // not passthrough because this is different from vanilla
                auto t = m_selectedTab - 1;
                if (t < 0) {
                    t = m_tabsArray->count() - 1;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("robtop.geometry-dash/next-build-tab", [this] {
                // not passthrough because this is different from vanilla
                auto t = m_selectedTab + 1;
                if (t > static_cast<int>(m_tabsArray->count() - 1)) {
                    t = 0;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("robtop.geometry-dash/next-layer", [this] {
                this->passThroughKeyDown(KEY_Right);
            });
            this->defineKeybind("robtop.geometry-dash/prev-layer", [this] {
                this->passThroughKeyDown(KEY_Left);
            });
            this->defineKeybind("robtop.geometry-dash/scroll-up", [this] {
                this->moveGamelayer({ .0f, 10.f });
            });
            this->defineKeybind("robtop.geometry-dash/scroll-down", [this] {
                this->moveGamelayer({ .0f, -10.f });
            });
            this->defineKeybind("robtop.geometry-dash/zoom-in", [this] {
                this->zoomIn(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/zoom-out", [this] {
                this->zoomOut(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-left", [this] {
                this->passThroughKeyDown(KEY_A);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-right", [this] {
                this->passThroughKeyDown(KEY_D);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-up", [this] {
                this->passThroughKeyDown(KEY_W);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-down", [this] {
                this->passThroughKeyDown(KEY_S);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-left-small", [this] {
                this->passThroughKeyDown(KEY_A, Modifier::Shift);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-right-small", [this] {
                this->passThroughKeyDown(KEY_D, Modifier::Shift);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-up-small", [this] {
                this->passThroughKeyDown(KEY_W, Modifier::Shift);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-down-small", [this] {
                this->passThroughKeyDown(KEY_S, Modifier::Shift);
            });
            this->defineKeybind("robtop.geometry-dash/lock-preview", [this] {
                this->passThroughKeyDown(KEY_F1);
            });
            this->defineKeybind("robtop.geometry-dash/unlock-preview", [this] {
                this->passThroughKeyDown(KEY_F2);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-preview-mode", [this] {
                this->passThroughKeyDown(KEY_F3);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-particle-icons", [this] {
                this->passThroughKeyDown(KEY_F4);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-editor-hitboxes", [this] {
                this->passThroughKeyDown(KEY_F5);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-hide-invisible", [this] {
                this->passThroughKeyDown(KEY_F6);
            });
            for (size_t i = 0; i < 10; i += 1) {
                auto x = std::to_string(i);
                auto key = static_cast<enumKeyCodes>(KEY_Zero + i);
                this->defineKeybind("robtop.geometry-dash/save-editor-position-" + x, [this, key] {
                    this->passThroughKeyDown(key, Modifier::PlatformControl);
                });
                this->defineKeybind("robtop.geometry-dash/load-editor-position-" + x, [this, key] {
                    this->passThroughKeyDown(key, Modifier::Alt);
                });
            }
            this->defineKeybind("robtop.geometry-dash/pan-editor", [this, lel](bool down) {
                if (lel->m_playbackMode != PlaybackMode::Playing) {
                    s_allowPassThrough = true;
                    if (down) {
                        this->keyDown(KEY_Space);
                    } else {
                        this->keyUp(KEY_Space);
                    }
                }
            });
         });

        return true;
    }

    void transformObjectCall(EditCommand p0) {
        return EditorUI::transformObjectCall(p0);
    }

    void moveObjectCall(EditCommand p0) {
        return EditorUI::moveObjectCall(p0);
    }

    void defineKeybind(std::string const& id, std::function<void(bool)> callback) {
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            callback(event->isDown());
            return ListenerResult::Propagate;
        }, id);
    }

    void defineKeybind(std::string const& id, std::function<void()> callback) {
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                callback();
            }
            return ListenerResult::Propagate;
        }, id);
    }

    static inline bool s_allowPassThrough = false;

    void passThroughKeyDown(enumKeyCodes key, Modifier modifiers = Modifier::None) {
        s_allowPassThrough = true;
        auto d = static_cast<EvilBypass*>(CCKeyboardDispatcher::get());
        auto alt = d->setAltPressed(modifiers & Modifier::Alt);
        auto shift = d->setShiftPressed(modifiers & Modifier::Shift);
        auto ctrl = d->setControlPressed(modifiers & Modifier::Control);
        auto cmd = d->setCommandPressed(modifiers & Modifier::Command);
        this->keyDown(key);
        d->setAltPressed(alt);
        d->setShiftPressed(shift);
        d->setControlPressed(ctrl);
        d->setCommandPressed(cmd);
    }

    void keyDown(enumKeyCodes key) {
        if (key == enumKeyCodes::KEY_Escape) {
            EditorUI::keyDown(key);
        }
        else if (s_allowPassThrough) {
            s_allowPassThrough = false;
            EditorUI::keyDown(key);
        }
    }
    void keyUp(enumKeyCodes key) {
        if (s_allowPassThrough) {
            s_allowPassThrough = false;
            EditorUI::keyUp(key);
        }
    }
};

$execute {
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/build-mode",
        "Build Mode",
        "Toggle the Build Tab",
        { Keybind::create(KEY_One, Modifier::None) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/edit-mode",
        "Edit Mode",
        "Toggle the Edit Tab",
        { Keybind::create(KEY_Two, Modifier::None) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/delete-mode",
        "Delete Mode",
        "Toggle the Delete Tab",
        { Keybind::create(KEY_Three, Modifier::None) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/rotate-ccw",
        "Rotate CCW",
        "Rotate Object(s) Counter-Clockwise",
        { Keybind::create(KEY_Q, Modifier::None) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/rotate-cw",
        "Rotate CW",
        "Rotate Object(s) Clockwise",
        { Keybind::create(KEY_E, Modifier::None) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/flip-x",
        "Flip X",
        "Flip Object(s) Along the X-Axis",
        { Keybind::create(KEY_Q, Modifier::Alt) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/flip-y",
        "Flip Y",
        "Flip Object(s) Along the Y-Axis",
        { Keybind::create(KEY_E, Modifier::Alt) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/delete",
        "Delete Selected",
        "Delete Selected Object(s)",
        { Keybind::create(KEY_Delete, Modifier::None) },
        Category::EDITOR_MODIFY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/undo",
        "Undo",
        "Undo Last Action",
        { Keybind::create(KEY_Z, Modifier::PlatformControl) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/redo",
        "Redo",
        "Redo Last Action",
        { Keybind::create(KEY_Z, Modifier::PlatformControl | Modifier::Shift) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/deselect-all",
        "Deselect All",
        "Deselect All Objects",
        { Keybind::create(KEY_D, Modifier::Alt) },
        Category::EDITOR_MODIFY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/copy",
        "Copy",
        "Copy Selected Objects",
        { Keybind::create(KEY_C, Modifier::PlatformControl) },
        Category::EDITOR_MODIFY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/paste",
        "Paste",
        "Paste Selected Objects",
        { Keybind::create(KEY_V, Modifier::PlatformControl) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/copy-paste",
        "Copy + Paste",
        "Duplicate Selected Objects",
        { Keybind::create(KEY_D, Modifier::PlatformControl) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-rotate",
        "Rotate",
        "Toggle Rotate Control",
        { Keybind::create(KEY_R, Modifier::None) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-transform",
        "Transform",
        "Toggle Transform Control",
        { Keybind::create(KEY_T, Modifier::PlatformControl) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-free-move",
        "Free Move",
        "Toggle Free Move",
        { Keybind::create(KEY_F, Modifier::None) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-swipe",
        "Swipe",
        "Toggle Swipe",
        { Keybind::create(KEY_T, Modifier::None) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-snap",
        "Snap",
        "Toggle Snap",
        { Keybind::create(KEY_G, Modifier::None) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/playtest",
        "Playtest",
        "Start / Stop Playtesting",
        { Keybind::create(KEY_Enter, Modifier::None), ControllerBind::create(CONTROLLER_Start) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/playback-music",
        "Playback Music",
        "Start / Stop Playing the Level's Music",
        { Keybind::create(KEY_Enter, Modifier::PlatformControl) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/prev-build-tab",
        "Previous Build Tab",
        "",
        {},
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/next-build-tab",
        "Next Build Tab",
        "",
        {},
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/next-layer",
        "Next Layer",
        "Go to Next Editor Layer",
        { Keybind::create(KEY_Right, Modifier::None) },
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/prev-layer",
        "Previous Layer",
        "Go to Previous Editor Layer",
        { Keybind::create(KEY_Left, Modifier::None) },
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/scroll-up",
        "Scroll Up",
        "",
        { Keybind::create(KEY_OEMPlus, Modifier::None) },
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/scroll-down",
        "Scroll Down",
        "",
        { Keybind::create(KEY_OEMMinus, Modifier::None) },
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/zoom-in",
        "Zoom In",
        "",
        { Keybind::create(KEY_OEMPlus, Modifier::Shift) },
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/zoom-out",
        "Zoom Out",
        "",
        { Keybind::create(KEY_OEMMinus, Modifier::Shift) },
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/lock-preview",
        "Lock Preview",
        "Locks the editor preview line at the center of the camera",
        { Keybind::create(KEY_F1) },
        Category::EDITOR_UI,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/unlock-preview",
        "Unlock Preview",
        "Unlocks the editor preview line",
        { Keybind::create(KEY_F2) },
        Category::EDITOR_UI,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-preview-mode",
        "Toggle Preview Mode",
        "Toggles preview mode ON/OFF",
        { Keybind::create(KEY_F3) },
        Category::EDITOR_UI,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-particle-icons",
        "Toggle Particle Icons",
        "Toggles between showing particles or showing their icons",
        { Keybind::create(KEY_F4) },
        Category::EDITOR_UI,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-editor-hitboxes",
        "Toggle Editor Hitboxes",
        "Toggles hitboxes in the editor",
        { Keybind::create(KEY_F5) },
        Category::EDITOR_UI,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-hide-invisible",
        "Toggle Hide Invisible",
        "Toggles visibility of hidden objects",
        { Keybind::create(KEY_F6) },
        Category::EDITOR_UI,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-left",
        "Move Object Left",
        "Move Selected Object(s) Left 1 Block (30 Units)",
        { Keybind::create(KEY_A, Modifier::None) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-right",
        "Move Object Right",
        "Move Selected Object(s) Right 1 Block (30 Units)",
        { Keybind::create(KEY_D, Modifier::None) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-up",
        "Move Object Up",
        "Move Selected Object(s) Up 1 Block (30 Units)",
        { Keybind::create(KEY_W, Modifier::None) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-down",
        "Move Object Down",
        "Move Selected Object(s) Down 1 Block (30 Units)",
        { Keybind::create(KEY_S, Modifier::None) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-left-small",
        "Move Object Left Small",
        "Move Selected Object(s) Left 2 Units",
        { Keybind::create(KEY_A, Modifier::Shift) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-right-small",
        "Move Object Right Small",
        "Move Selected Object(s) Right 2 Units",
        { Keybind::create(KEY_D, Modifier::Shift) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-up-small",
        "Move Object Up Small",
        "Move Selected Object(s) Up 2 Units",
        { Keybind::create(KEY_W, Modifier::Shift) },
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-down-small",
        "Move Object Down Small",
        "Move Selected Object(s) Down 2 Units",
        { Keybind::create(KEY_S, Modifier::Shift) },
        Category::EDITOR_MOVE, true
    });
    for (size_t i = 0; i < 10; i += 1) {
        auto x = std::to_string(i);
        BindManager::get()->registerBindable({
            "robtop.geometry-dash/save-editor-position-" + x,
            "Save Editor Position " + x,
            "Save the current editor camera position in the slot " + x + ". "
            "You can reload this slot back with Load Editor Position " + x,
            { Keybind::create(static_cast<enumKeyCodes>(KEY_Zero + i), Modifier::PlatformControl) },
            Category::EDITOR_UI, false
        });
        BindManager::get()->registerBindable({
            "robtop.geometry-dash/load-editor-position-" + x,
            "Load Editor Position " + x,
            "Load the current editor camera position in the slot " + x,
            { Keybind::create(static_cast<enumKeyCodes>(KEY_Zero + i), Modifier::Alt) },
            Category::EDITOR_UI, false
        });
    }
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/pan-editor",
        "Pan Editor While Swiping",
        "Allows you to pan in the editor if you have swipe enabled",
        { Keybind::create(KEY_Space) },
        Category::EDITOR_UI, false
    });
}
