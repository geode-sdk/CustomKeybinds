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

        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), "unpause-level"),
            [this](Keybind const& id, bool down, bool repeat, double timestamp) {
                if (!repeat && down) {
                    this->onResume(nullptr);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            }
        );
    }

    void keyDown(enumKeyCodes key, double timestamp) {
        if (key == enumKeyCodes::KEY_Escape) {
            EditorPauseLayer::keyDown(key, timestamp);
        }
    }
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
            this->defineKeybind("jump-p1", [lel](bool down, bool repeat, double timestamp) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(platformButton(), down, false, timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("jump-p2", [lel](bool down, bool repeat, double timestamp) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(platformButton(), down, true, timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-left-p1", [lel](bool down, bool repeat, double timestamp) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Left), down, false, timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-right-p1", [lel](bool down, bool repeat, double timestamp) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Right), down, false, timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-left-p2", [lel](bool down, bool repeat, double timestamp) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Left), down, true, timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("move-right-p2", [lel](bool down, bool repeat, double timestamp) {
                if (!repeat && lel->m_playbackMode == PlaybackMode::Playing) {
                    lel->queueButton(static_cast<int>(PlayerButton::Right), down, true, timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("build-mode", [this](double timestamp) {
                this->passThroughKeyDown(KEY_One, timestamp);
            });
            this->defineKeybind("edit-mode", [this](double timestamp) {
                this->passThroughKeyDown(KEY_Two, timestamp);
            });
            this->defineKeybind("delete-mode", [this](double timestamp) {
                this->passThroughKeyDown(KEY_Three, timestamp);
            });
            this->defineKeybind("rotate-ccw", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_Q, timestamp);
            });
            this->defineKeybind("rotate-cw", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_E, timestamp);
            });
            this->defineKeybind("flip-x", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_Q, timestamp, Keybind::Mods_Alt);
            });
            this->defineKeybind("flip-y", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_E, timestamp, Keybind::Mods_Alt);
            });
            this->defineKeybind("delete", [this](double timestamp) {
                this->passThroughKeyDown(KEY_Delete, timestamp);
            });
            this->defineKeybind("undo", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_Z, timestamp, Keybind::Mods_Control);
            });
            this->defineKeybind("redo", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_Z, timestamp, Keybind::Mods_Control | Keybind::Mods_Shift);
            });
            this->defineKeybind("deselect-all", [this](double timestamp) {
                this->passThroughKeyDown(KEY_D, timestamp, Keybind::Mods_Alt);
            });
            this->defineKeybind("copy", [this](double timestamp) {
                this->passThroughKeyDown(KEY_C, timestamp, Keybind::Mods_Control);
            });
            this->defineKeybind("paste", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_V, timestamp, Keybind::Mods_Control);
            });
            this->defineKeybind("copy-paste", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_D, timestamp, Keybind::Mods_Control);
            });
            this->defineKeybind("toggle-rotate", [this](double timestamp) {
                this->passThroughKeyDown(KEY_R, timestamp);
            });
            this->defineKeybind("toggle-transform", [this](double timestamp) {
                this->passThroughKeyDown(KEY_T, timestamp, Keybind::Mods_Control);
            });
            this->defineKeybind("toggle-free-move", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F, timestamp);
            });
            this->defineKeybind("toggle-swipe", [this](double timestamp) {
                this->passThroughKeyDown(KEY_T, timestamp);
            });
            this->defineKeybind("toggle-snap", [this](double timestamp) {
                this->passThroughKeyDown(KEY_G, timestamp);
            });
            this->defineKeybind("playtest", [this](double timestamp) {
                this->passThroughKeyDown(KEY_Enter, timestamp);
            });
            this->defineKeybind("playback-music", [this](double timestamp) {
                // RobTop broke this in 2.2, which makes it trigger the playtest keybind
                // this->passThroughKeyDown(KEY_Enter, Keybind::Mods_Control);
                EditorUI::onPlayback(nullptr);
            });
            this->defineKeybind("prev-build-tab", [this](bool repeat, double timestamp) {
                // not passthrough because this is different from vanilla
                auto t = m_selectedTab - 1;
                if (t < 0) {
                    t = m_tabsArray->count() - 1;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("next-build-tab", [this](bool repeat, double timestamp) {
                // not passthrough because this is different from vanilla
                auto t = m_selectedTab + 1;
                if (t > static_cast<int>(m_tabsArray->count() - 1)) {
                    t = 0;
                }
                this->selectBuildTab(t);
            });
            this->defineKeybind("next-layer", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_Right, timestamp);
            });
            this->defineKeybind("prev-layer", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_Left, timestamp);
            });
            this->defineKeybind("scroll-up", [this](bool repeat, double timestamp) {
                this->moveGamelayer({ .0f, 10.f });
            });
            this->defineKeybind("scroll-down", [this](bool repeat, double timestamp) {
                this->moveGamelayer({ .0f, -10.f });
            });
            this->defineKeybind("zoom-in", [this](bool repeat, double timestamp) {
                this->zoomIn(nullptr);
            });
            this->defineKeybind("zoom-out", [this](bool repeat, double timestamp) {
                this->zoomOut(nullptr);
            });
            this->defineKeybind("move-obj-left", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_A, timestamp);
            });
            this->defineKeybind("move-obj-right", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_D, timestamp);
            });
            this->defineKeybind("move-obj-up", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_W, timestamp);
            });
            this->defineKeybind("move-obj-down", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_S, timestamp);
            });
            this->defineKeybind("move-obj-left-small", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_A, timestamp, Keybind::Mods_Shift);
            });
            this->defineKeybind("move-obj-right-small", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_D, timestamp, Keybind::Mods_Shift);
            });
            this->defineKeybind("move-obj-up-small", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_W, timestamp, Keybind::Mods_Shift);
            });
            this->defineKeybind("move-obj-down-small", [this](bool repeat, double timestamp) {
                this->passThroughKeyDown(KEY_S, timestamp, Keybind::Mods_Shift);
            });
            this->defineKeybind("lock-preview", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F1, timestamp);
            });
            this->defineKeybind("unlock-preview", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F2, timestamp);
            });
            this->defineKeybind("toggle-preview-mode", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F3, timestamp);
            });
            this->defineKeybind("toggle-particle-icons", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F4, timestamp);
            });
            this->defineKeybind("toggle-editor-hitboxes", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F5, timestamp);
            });
            this->defineKeybind("toggle-hide-invisible", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F6, timestamp);
            });
            this->defineKeybind("toggle-link-controls", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F7, timestamp);
            });
            this->defineKeybind("toggle-trace-in", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F8, timestamp);
            });
            this->defineKeybind("toggle-trace-out", [this](double timestamp) {
                this->passThroughKeyDown(KEY_F9, timestamp);
            });
            for (size_t i = 0; i < 10; i += 1) {
                auto key = static_cast<enumKeyCodes>(KEY_Zero + i);
                this->defineKeybind(fmt::format("save-editor-position-{}", i), [this, key](double timestamp) {
                    this->passThroughKeyDown(key, timestamp, Keybind::Mods_Control);
                });
                this->defineKeybind(fmt::format("load-editor-position-{}", i), [this, key](double timestamp) {
                    this->passThroughKeyDown(key, timestamp, Keybind::Mods_Alt);
                });
            }
            this->defineKeybind("pan-editor", [this, lel](bool down, bool repeat, double timestamp) {
                if (!repeat && lel->m_playbackMode != PlaybackMode::Playing) {
                    s_allowPassThrough = true;
                    if (down) {
                        this->keyDown(KEY_Space, timestamp);
                    } else {
                        this->keyUp(KEY_Space, timestamp);
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

    void defineKeybind(std::string id, CopyableFunction<bool(bool, bool, double)> callback) {
        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), std::move(id)),
            [callback = std::move(callback)](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                return callback(down, repeat, timestamp);
            }
        );
    }

    void defineKeybind(std::string id, CopyableFunction<void(bool, double)> callback) {
        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), std::move(id)),
            [callback = std::move(callback)](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (down) {
                    callback(repeat, timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            }
        );
    }

    void defineKeybind(std::string id, CopyableFunction<void(double)> callback) {
        this->addEventListener(
            KeybindSettingPressedEventV3(Mod::get(), std::move(id)),
            [callback = std::move(callback)](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (!repeat && down) {
                    callback(timestamp);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            }
        );
    }

    static inline bool s_allowPassThrough = false;

    void passThroughKeyDown(enumKeyCodes key, double timestamp, Keybind::Modifiers modifiers = Keybind::Mods_None) {
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
        this->keyDown(key, timestamp);
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
