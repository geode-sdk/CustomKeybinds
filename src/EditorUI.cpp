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

#if defined(GEODE_IS_MACOS)
    static constexpr auto PLATFORM_CONTROL = Modifier::Command;
#else
    static constexpr auto PLATFORM_CONTROL = Modifier::Control;
#endif

struct $modify(EditorPauseLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorPauseLayer::keyDown", 1000);
    }

    void customSetup() {
        EditorPauseLayer::customSetup();

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
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

std::unordered_set<enumKeyCodes> g_allowedKeyCodes;

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

        Loader::get()->queueInMainThread([this] {
            this->defineKeybind("robtop.geometry-dash/jump-p1", [=](bool down) {
                m_editorLayer->queueButton(platformButton(), down, false);
            });
            this->defineKeybind("robtop.geometry-dash/jump-p2", [=](bool down) {
                m_editorLayer->queueButton(platformButton(), down, true);
            });
            this->defineKeybind("robtop.geometry-dash/move-left-p1", [this](bool down) {
                m_editorLayer->queueButton(static_cast<int>(PlayerButton::Left), down, false);
            });
            this->defineKeybind("robtop.geometry-dash/move-right-p1", [this](bool down) {
                m_editorLayer->queueButton(static_cast<int>(PlayerButton::Right), down, false);
            });
            this->defineKeybind("robtop.geometry-dash/move-left-p2", [this](bool down) {
                m_editorLayer->queueButton(static_cast<int>(PlayerButton::Left), down, true);
            });
            this->defineKeybind("robtop.geometry-dash/move-right-p2", [this](bool down) {
                m_editorLayer->queueButton(static_cast<int>(PlayerButton::Right), down, true);
            });
            this->defineKeybind("robtop.geometry-dash/pause-level", [=](bool down) {
                if(down && !getChildOfType<EditorPauseLayer>(this->getParent(), 0)) {
                    EditorUI::onPause(nullptr);
                };
            });
            this->defineKeybind("robtop.geometry-dash/build-mode", [=] {
                this->toggleMode(m_buildModeBtn);
            });
            this->defineKeybind("robtop.geometry-dash/edit-mode", [=] {
                this->toggleMode(m_editModeBtn);
            });
            this->defineKeybind("robtop.geometry-dash/delete-mode", [=] {
                this->toggleMode(m_deleteModeBtn);
            });
            this->defineKeybind("robtop.geometry-dash/rotate-ccw", [=] {
                this->transformObjectCall(EditCommand::RotateCCW);
            });
            this->defineKeybind("robtop.geometry-dash/rotate-cw", [=] {
                this->transformObjectCall(EditCommand::RotateCW);
            });
            this->defineKeybind("robtop.geometry-dash/flip-x", [=] {
                this->transformObjectCall(EditCommand::FlipX);
            });
            this->defineKeybind("robtop.geometry-dash/flip-y", [=] {
                this->transformObjectCall(EditCommand::FlipY);
            });
            this->defineKeybind("robtop.geometry-dash/delete", [=] {
                this->onDeleteSelected(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/undo", [=] {
                this->undoLastAction(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/redo", [=] {
                this->redoLastAction(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/deselect-all", [=] {
                this->deselectAll();
            });
            this->defineKeybind("robtop.geometry-dash/copy", [=] {
                this->onCopy(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/paste", [=] {
                this->onPaste(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/copy-paste", [=] {
                this->onDuplicate(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-rotate", [=] {
                this->toggleEnableRotate(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-free-move", [=] {
                this->toggleFreeMove(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-swipe", [=] {
                this->toggleSwipe(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-snap", [=] {
                this->toggleSnap(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/playtest", [=] {
                if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
                    this->onStopPlaytest(nullptr);
                }
                else {
                    this->onPlaytest(nullptr);
                }
            });
            this->defineKeybind("robtop.geometry-dash/playback-music", [=] {
                if (m_editorLayer->m_playbackMode != PlaybackMode::Playing) {
                    this->onPlayback(nullptr);
                }
            });
            this->defineKeybind("robtop.geometry-dash/prev-build-tab", [=] {
                auto t = m_selectedTab - 1;
                if (t < 0) {
                    t = m_tabsArray->count() - 1;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("robtop.geometry-dash/next-build-tab", [=] {
                auto t = m_selectedTab + 1;
                if (t > static_cast<int>(m_tabsArray->count() - 1)) {
                    t = 0;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("robtop.geometry-dash/next-layer", [=] {
                this->onGroupUp(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/prev-layer", [=] {
                this->onGroupDown(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/scroll-up", [=] {
                this->moveGamelayer({ .0f, 10.f });
            });
            this->defineKeybind("robtop.geometry-dash/scroll-down", [=] {
                this->moveGamelayer({ .0f, -10.f });
            });
            this->defineKeybind("robtop.geometry-dash/zoom-in", [=] {
                this->zoomIn(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/zoom-out", [=] {
                this->zoomOut(nullptr);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-left", [=] {
                this->moveObjectCall(EditCommand::Left);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-right", [=] {
                this->moveObjectCall(EditCommand::Right);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-up", [=] {
                this->moveObjectCall(EditCommand::Up);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-down", [=] {
                this->moveObjectCall(EditCommand::Down);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-left-small", [=] {
                this->moveObjectCall(EditCommand::SmallLeft);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-right-small", [=] {
                this->moveObjectCall(EditCommand::SmallRight);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-up-small", [=] {
                this->moveObjectCall(EditCommand::SmallUp);
            });
            this->defineKeybind("robtop.geometry-dash/move-obj-down-small", [=] {
                this->moveObjectCall(EditCommand::SmallDown);
            });
            this->defineKeybind("robtop.geometry-dash/lock-preview", [=] {
                g_allowedKeyCodes.insert(KEY_F1);
                this->keyDown(KEY_F1);
            });
            this->defineKeybind("robtop.geometry-dash/unlock-preview", [=] {
                g_allowedKeyCodes.insert(KEY_F2);
                this->keyDown(KEY_F2);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-preview-mode", [=] {
                g_allowedKeyCodes.insert(KEY_F3);
                this->keyDown(KEY_F3);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-particle-icons", [=] {
                g_allowedKeyCodes.insert(KEY_F4);
                this->keyDown(KEY_F4);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-editor-hitboxes", [=] {
                g_allowedKeyCodes.insert(KEY_F5);
                this->keyDown(KEY_F5);
            });
            this->defineKeybind("robtop.geometry-dash/toggle-hide-invisible", [=] {
                g_allowedKeyCodes.insert(KEY_F6);
                this->keyDown(KEY_F6);
            });
        });

        return true;
    }

    TodoReturn transformObjectCall(EditCommand p0) {
        log::info("{}", static_cast<int>(p0));
        return EditorUI::transformObjectCall(p0);
    }

    TodoReturn moveObjectCall(EditCommand p0) {
        log::info("{}", static_cast<int>(p0));
        return EditorUI::moveObjectCall(p0);
    }

    void defineKeybind(const char* id, std::function<void(bool)> callback) {
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            callback(event->isDown());
            return ListenerResult::Propagate;
        }, id);
    }

    void defineKeybind(const char* id, std::function<void()> callback) {
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                callback();
            }
            return ListenerResult::Propagate;
        }, id);
    }

    void keyDown(enumKeyCodes key) {
        // TODO: I'll let F keys through until we figure out how to toggle them
        if (key == enumKeyCodes::KEY_Escape || (key >= enumKeyCodes::KEY_F1 && key <= enumKeyCodes::KEY_F6)) {
            EditorUI::keyDown(key);
        }
        if (g_allowedKeyCodes.contains(key)) {
            g_allowedKeyCodes.erase(key);
            EditorUI::keyDown(key);
        }
    }
    void keyUp(enumKeyCodes) {}
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
        { Keybind::create(KEY_Z, PLATFORM_CONTROL) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/redo",
        "Redo",
        "Redo Last Action",
        { Keybind::create(KEY_Z, PLATFORM_CONTROL | Modifier::Shift) },
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
        { Keybind::create(KEY_C, PLATFORM_CONTROL) },
        Category::EDITOR_MODIFY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/paste",
        "Paste",
        "Paste Selected Objects",
        { Keybind::create(KEY_V, PLATFORM_CONTROL) },
        Category::EDITOR_MODIFY, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/copy-paste",
        "Copy + Paste",
        "Duplicate Selected Objects",
        { Keybind::create(KEY_D, PLATFORM_CONTROL) },
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
        { Keybind::create(KEY_Enter, Modifier::None), ControllerBind::create(CONTROLLER_Y) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/playback-music",
        "Playback Music",
        "Start / Stop Playing the Level's Music",
        { Keybind::create(KEY_Enter, PLATFORM_CONTROL) },
        Category::EDITOR_UI, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/prev-build-tab",
        "Previous Build Tab",
        "",
        { Keybind::create(KEY_F1, Modifier::None) },
        Category::EDITOR_UI, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/next-build-tab",
        "Next Build Tab",
        "",
        { Keybind::create(KEY_F2, Modifier::None) },
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
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-half-left",
        "Move Object Half Left",
        "Move Selected Object(s) half of a Block left (15 Units)",
        {Keybind::create(KEY_A, Modifier::Control | Modifier::Alt)},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-half-right",
        "Move Object Half Right",
        "Move Selected Object(s) half of a Block right (15 Units)",
        {Keybind::create(KEY_D, Modifier::Control | Modifier::Alt)},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-half-up",
        "Move Object Half Up",
        "Move Selected Object(s) half of a Block up (15 Units)",
        {Keybind::create(KEY_W, Modifier::Control | Modifier::Alt)},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-half-down",
        "Move Object Half Down",
        "Move Selected Object(s) half of a Block down (15 Units)",
        {Keybind::create(KEY_S, Modifier::Control | Modifier::Alt)},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-big-left",
        "Move Object Big Left",
        "Move Selected Object(s) 5 Blocks left (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-big-right",
        "Move Object Big Right",
        "Move Selected Object(s) 5 Blocks right (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-big-up",
        "Move Object Big Up",
        "Move Selected Object(s) 5 Blocks up (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-obj-big-down",
        "Move Object Big Down",
        "Move Selected Object(s) 5 Blocks down (150 Units)",
        {},
        Category::EDITOR_MOVE, true
    });
}
