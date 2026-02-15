#include "timestamp.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

struct $modify(EditorPauseLayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorPauseLayer::keyDown", Priority::Late);
    }

    void customSetup() {
        EditorPauseLayer::customSetup();

        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "unpause-level"), [this](Keybind const& id, bool down, bool repeat) {
            if (!repeat && down) {
                this->onResume(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        });
    }

    void keyDown(enumKeyCodes key, double timestamp) {
        if (key == enumKeyCodes::KEY_Escape) {
            EditorPauseLayer::keyDown(key, timestamp);
        }
    }

    // This function is merged with MoreOptionsLayer::onKeybindings on Windows, which is already hooked
    #if !defined(GEODE_IS_WINDOWS) && !defined(GEODE_IS_IOS)
    void onKeybindings(CCObject*) {
        openSettingsPopup(Mod::get(), false);
    }
    #endif
};

struct $modify(EditorUI) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("EditorUI::keyDown", Priority::Replace);
        (void)self.setHookPriority("EditorUI::keyUp", Priority::Late);
    }

    static inline int platformButton() {
        return 1;
    }

    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel))
            return false;

        Loader::get()->queueInMainThread([this, lel] {
            this->defineKeybind("jump-p1", [lel](bool down, bool repeat) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(platformButton(), down, false, getCurrentTimestamp());
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("jump-p2", [lel](bool down, bool repeat) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(platformButton(), down, true, getCurrentTimestamp());
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-left-p1", [lel](bool down, bool repeat) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Left), down, false, getCurrentTimestamp());
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-right-p1", [lel](bool down, bool repeat) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Right), down, false, getCurrentTimestamp());
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-left-p2", [lel](bool down, bool repeat) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Left), down, true, getCurrentTimestamp());
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-right-p2", [lel](bool down, bool repeat) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Right), down, true, getCurrentTimestamp());
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("build-mode", [this] {
                this->passThroughKeyDown(KEY_One);
            });
            this->defineKeybind("edit-mode", [this] {
                this->passThroughKeyDown(KEY_Two);
            });
            this->defineKeybind("delete-mode", [this] {
                this->passThroughKeyDown(KEY_Three);
            });
            this->defineKeybind("rotate-ccw", [this](bool repeat) {
                this->passThroughKeyDown(KEY_Q);
            });
            this->defineKeybind("rotate-cw", [this](bool repeat) {
                this->passThroughKeyDown(KEY_E);
            });
            this->defineKeybind("flip-x", [this](bool repeat) {
                this->passThroughKeyDown(KEY_Q, Keybind::Mods_Alt);
            });
            this->defineKeybind("flip-y", [this](bool repeat) {
                this->passThroughKeyDown(KEY_E, Keybind::Mods_Alt);
            });
            this->defineKeybind("delete", [this] {
                this->passThroughKeyDown(KEY_Delete);
            });
            this->defineKeybind("undo", [this](bool repeat) {
                this->passThroughKeyDown(KEY_Z, Keybind::Mods_Control);
            });
            this->defineKeybind("redo", [this](bool repeat) {
                this->passThroughKeyDown(KEY_Z, Keybind::Mods_Control | Keybind::Mods_Shift);
            });
            this->defineKeybind("deselect-all", [this] {
                this->passThroughKeyDown(KEY_D, Keybind::Mods_Alt);
            });
            this->defineKeybind("copy", [this] {
                this->passThroughKeyDown(KEY_C, Keybind::Mods_Control);
            });
            this->defineKeybind("paste", [this](bool repeat) {
                this->passThroughKeyDown(KEY_V, Keybind::Mods_Control);
            });
            this->defineKeybind("copy-paste", [this](bool repeat) {
                this->passThroughKeyDown(KEY_D, Keybind::Mods_Control);
            });
            this->defineKeybind("toggle-rotate", [this] {
                this->passThroughKeyDown(KEY_R);
            });
            this->defineKeybind("toggle-transform", [this] {
                this->passThroughKeyDown(KEY_T, Keybind::Mods_Control);
            });
            this->defineKeybind("toggle-free-move", [this] {
                this->passThroughKeyDown(KEY_F);
            });
            this->defineKeybind("toggle-swipe", [this] {
                this->passThroughKeyDown(KEY_T);
            });
            this->defineKeybind("toggle-snap", [this] {
                this->passThroughKeyDown(KEY_G);
            });
            this->defineKeybind("playtest", [this] {
                this->passThroughKeyDown(KEY_Enter);
            });
            this->defineKeybind("playback-music", [this] {
                // RobTop broke this in 2.2, which makes it trigger the playtest keybind
                // this->passThroughKeyDown(KEY_Enter, Keybind::Mods_Control);
                EditorUI::onPlayback(nullptr);
            });
            this->defineKeybind("prev-build-tab", [this](bool repeat) {
                // not passthrough because this is different from vanilla
                auto t = m_selectedTab - 1;
                if (t < 0) {
                    t = m_tabsArray->count() - 1;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("next-build-tab", [this](bool repeat) {
                // not passthrough because this is different from vanilla
                auto t = m_selectedTab + 1;
                if (t > static_cast<int>(m_tabsArray->count() - 1)) {
                    t = 0;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("next-layer", [this](bool repeat) {
                this->passThroughKeyDown(KEY_Right);
            });
            this->defineKeybind("prev-layer", [this](bool repeat) {
                this->passThroughKeyDown(KEY_Left);
            });
            this->defineKeybind("scroll-up", [this](bool repeat) {
                this->moveGamelayer({ .0f, 10.f });
            });
            this->defineKeybind("scroll-down", [this](bool repeat) {
                this->moveGamelayer({ .0f, -10.f });
            });
            this->defineKeybind("zoom-in", [this](bool repeat) {
                this->zoomIn(nullptr);
            });
            this->defineKeybind("zoom-out", [this](bool repeat) {
                this->zoomOut(nullptr);
            });
            this->defineKeybind("move-obj-left", [this](bool repeat) {
                this->passThroughKeyDown(KEY_A);
            });
            this->defineKeybind("move-obj-right", [this](bool repeat) {
                this->passThroughKeyDown(KEY_D);
            });
            this->defineKeybind("move-obj-up", [this](bool repeat) {
                this->passThroughKeyDown(KEY_W);
            });
            this->defineKeybind("move-obj-down", [this](bool repeat) {
                this->passThroughKeyDown(KEY_S);
            });
            this->defineKeybind("move-obj-left-small", [this](bool repeat) {
                this->passThroughKeyDown(KEY_A, Keybind::Mods_Shift);
            });
            this->defineKeybind("move-obj-right-small", [this](bool repeat) {
                this->passThroughKeyDown(KEY_D, Keybind::Mods_Shift);
            });
            this->defineKeybind("move-obj-up-small", [this](bool repeat) {
                this->passThroughKeyDown(KEY_W, Keybind::Mods_Shift);
            });
            this->defineKeybind("move-obj-down-small", [this](bool repeat) {
                this->passThroughKeyDown(KEY_S, Keybind::Mods_Shift);
            });
            this->defineKeybind("lock-preview", [this] {
                this->passThroughKeyDown(KEY_F1);
            });
            this->defineKeybind("unlock-preview", [this] {
                this->passThroughKeyDown(KEY_F2);
            });
            this->defineKeybind("toggle-preview-mode", [this] {
                this->passThroughKeyDown(KEY_F3);
            });
            this->defineKeybind("toggle-particle-icons", [this] {
                this->passThroughKeyDown(KEY_F4);
            });
            this->defineKeybind("toggle-editor-hitboxes", [this] {
                this->passThroughKeyDown(KEY_F5);
            });
            this->defineKeybind("toggle-hide-invisible", [this] {
                this->passThroughKeyDown(KEY_F6);
            });
            for (size_t i = 0; i < 10; i += 1) {
                auto key = static_cast<enumKeyCodes>(KEY_Zero + i);
                this->defineKeybind(fmt::format("save-editor-position-{}", i), [this, key] {
                    this->passThroughKeyDown(key, Keybind::Mods_Control);
                });
                this->defineKeybind(fmt::format("load-editor-position-{}", i), [this, key] {
                    this->passThroughKeyDown(key, Keybind::Mods_Alt);
                });
            }
            this->defineKeybind("pan-editor", [this, lel](bool down, bool repeat) {
                if (!repeat && lel->m_playbackMode != PlaybackMode::Playing) {
                    s_allowPassThrough = true;
                    if (down) {
                        this->keyDown(KEY_Space, getCurrentTimestamp());
                    } else {
                        this->keyUp(KEY_Space, getCurrentTimestamp());
                    }
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
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

    void defineKeybind(std::string id, CopyableFunction<bool(bool, bool)> callback) {
        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), std::move(id)), [callback = std::move(callback)](Keybind const& keybind, bool down, bool repeat) {
            return callback(down, repeat);
        });
    }

    void defineKeybind(std::string id, CopyableFunction<void(bool)> callback) {
        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), std::move(id)), [callback = std::move(callback)](Keybind const& keybind, bool down, bool repeat) {
            if (down) {
                callback(repeat);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        });
    }

    void defineKeybind(std::string id, CopyableFunction<void()> callback) {
        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), std::move(id)), [callback = std::move(callback)](Keybind const& keybind, bool down, bool repeat) {
            if (!repeat && down) {
                callback();
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        });
    }

    static inline bool s_allowPassThrough = false;

    void passThroughKeyDown(enumKeyCodes key, Keybind::Modifiers modifiers = Keybind::Mods_None) {
        s_allowPassThrough = true;
        auto d = CCKeyboardDispatcher::get();
        auto alt = d->m_bAltPressed;
        auto shift = d->m_bShiftPressed;
        auto ctrl = d->m_bControlPressed;
        auto cmd = d->m_bCommandPressed;
        d->m_bAltPressed = modifiers & Keybind::Mods_Alt;
        d->m_bShiftPressed = modifiers & Keybind::Mods_Shift;
        d->m_bControlPressed = modifiers & Keybind::Mods_Control;
        d->m_bCommandPressed = modifiers & Keybind::Mods_Super;
        this->keyDown(key, getCurrentTimestamp());
        d->m_bAltPressed = alt;
        d->m_bShiftPressed = shift;
        d->m_bControlPressed = ctrl;
        d->m_bCommandPressed = cmd;
    }

    void keyDown(enumKeyCodes key, double timestamp) {
        if (key == enumKeyCodes::KEY_Escape) {
            EditorUI::keyDown(key, timestamp);
        }
        else if (s_allowPassThrough) {
            s_allowPassThrough = false;
            EditorUI::keyDown(key, timestamp);
        }
    }
    void keyUp(enumKeyCodes key, double timestamp) {
        if (s_allowPassThrough) {
            s_allowPassThrough = false;
            EditorUI::keyUp(key, timestamp);
        }
    }
};
