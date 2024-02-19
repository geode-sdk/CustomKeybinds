#include "Geode/binding/UILayer.hpp"
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include "../include/Keybinds.hpp"
#include "Geode/Enums.hpp"
#include "Geode/GeneratedPredeclare.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"

using namespace geode::prelude;
using namespace keybinds;

static void addBindSprites(CCNode* target, const char* action) {
    target->removeAllChildren();

    auto bindContainer = CCNode::create();
    bindContainer->setScale(.65f);
    bool first = true;
    for (auto& bind : BindManager::get()->getBindsFor(action)) {
        if (!first) {
            bindContainer->addChild(CCLabelBMFont::create("/", "bigFont.fnt"));
        }
        first = false;
        bindContainer->addChild(bind->createLabel());
    }
    bindContainer->setID("binds"_spr);
    bindContainer->setContentSize({
        target->getContentSize().width / bindContainer->getScale(), 40.f
    });
    bindContainer->setLayout(RowLayout::create());
    bindContainer->setAnchorPoint({ .5f, .5f });
    bindContainer->setPosition(target->getContentSize().width / 2, -1.f);
    target->addChild(bindContainer);
}

struct $modify(PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                this->onResume(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        }, "robtop.geometry-dash/unpause-level");

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                this->onQuit(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        }, "robtop.geometry-dash/exit-level");

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                if(PlayLayer::get() && PlayLayer::get()->m_isPracticeMode) {
                    this->onNormalMode(nullptr);
                } else {
                    this->onPracticeMode(nullptr);
                }
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        }, "robtop.geometry-dash/practice-level");

        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                this->onRestart(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        }, "robtop.geometry-dash/restart-level");
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            if (event->isDown()) {
                this->onRestartFull(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        }, "robtop.geometry-dash/full-restart-level");
    }

    void keyDown(enumKeyCodes key) {
        if (key == enumKeyCodes::KEY_Escape) {
            PauseLayer::keyDown(key);
        }
    }
};

struct $modify(UILayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("UILayer::keyDown", 1000);
        (void)self.setHookPriority("UILayer::keyUp", 1000);
    }

    static inline int platformButton() {
        return 1;
    }

    bool isPaused() {
        return getChildOfType<PauseLayer>(PlayLayer::get()->getParent(), 0) != nullptr;
    }

    bool isCurrentPlayLayer() {
        auto playLayer = getChildOfType<PlayLayer>(CCScene::get(), 0);
        return playLayer != nullptr && playLayer == PlayLayer::get() && getChildOfType<UILayer>(playLayer, 0) == this;
    }

    void pressKeyFallthrough(enumKeyCodes key, bool down) {
        // amazing hack
        if (this->isPaused())
            return;
        allowKeyDownThrough = true;
        if (down) {
            this->keyDown(key);
        } else {
            this->keyUp(key);
        }
        allowKeyDownThrough = false;
    }

    bool init(GJBaseGameLayer* layer) {
        if (!UILayer::init(layer))
            return false;

        // delay by a single frame
        geode::Loader::get()->queueInMainThread([this] {
            // do not do anything in the editor
            if (!PlayLayer::get()) return;

            this->defineKeybindStop("robtop.geometry-dash/jump-p1", [=](bool down) {
                this->pressKeyFallthrough(KEY_Space, down);
            });
            this->defineKeybindStop("robtop.geometry-dash/jump-p2", [=](bool down) {
                this->pressKeyFallthrough(KEY_Up, down);
            });
            this->defineKeybindStop("robtop.geometry-dash/place-checkpoint", [=](bool down) {
                this->pressKeyFallthrough(KEY_Z, down);
            });
            this->defineKeybindStop("robtop.geometry-dash/delete-checkpoint", [=](bool down) {
                this->pressKeyFallthrough(KEY_X, down);
            });
            this->defineKeybind("robtop.geometry-dash/pause-level", [=](bool down) {
                if (down && this->isCurrentPlayLayer() && !this->isPaused()) {
                    PlayLayer::get()->pauseGame(true);
                }
            });
            this->defineKeybind("robtop.geometry-dash/restart-level", [=](bool down) {
                if (down && this->isCurrentPlayLayer() && !this->isPaused() && PlayLayer::get()->canPauseGame()) {
                    PlayLayer::get()->resetLevel();
                }
            });
            this->defineKeybind("robtop.geometry-dash/full-restart-level", [=](bool down) {
                if (down && this->isCurrentPlayLayer() && !this->isPaused() && PlayLayer::get()->canPauseGame()) {
                    PlayLayer::get()->fullReset();
                }
            });
            this->defineKeybindStop("robtop.geometry-dash/move-left-p1", [=](bool down) {
                this->pressKeyFallthrough(KEY_A, down);
            });
            this->defineKeybindStop("robtop.geometry-dash/move-right-p1", [=](bool down) {
                this->pressKeyFallthrough(KEY_D, down);
            });
            this->defineKeybindStop("robtop.geometry-dash/move-left-p2", [=](bool down) {
                this->pressKeyFallthrough(KEY_Left, down);
            });
            this->defineKeybindStop("robtop.geometry-dash/move-right-p2", [=](bool down) {
                this->pressKeyFallthrough(KEY_Right, down);
            });
            this->defineKeybindStop("robtop.geometry-dash/toggle-hitboxes", [=](bool down) {
                if (down && this->isCurrentPlayLayer() && !this->isPaused()) {
                    // This assumes you have quick keys on
                    this->pressKeyFallthrough(KEY_P, down);
                }
            });
            // display practice mode button keybinds
            if (auto menu = this->getChildByID("checkpoint-menu")) {
                if (auto add = menu->getChildByID("add-checkpoint-button")) {
                    addBindSprites(
                        static_cast<CCMenuItemSpriteExtra*>(add)->getNormalImage(),
                        "robtop.geometry-dash/place-checkpoint"
                    );
                }
                if (auto rem = menu->getChildByID("remove-checkpoint-button")) {
                    addBindSprites(
                        static_cast<CCMenuItemSpriteExtra*>(rem)->getNormalImage(),
                        "robtop.geometry-dash/delete-checkpoint"
                    );
                }
            }
        });

        return true;
    }

    void defineKeybind(const char* id, std::function<void(bool)> callback) {
        // adding the events to playlayer instead
        PlayLayer::get()->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            callback(event->isDown());
            return ListenerResult::Propagate;
        }, id);
    }

    void defineKeybindStop(const char* id, std::function<void(bool)> callback) {
        // adding the events to playlayer instead
        PlayLayer::get()->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            callback(event->isDown());
            return ListenerResult::Stop;
        }, id);
    }

    static inline bool allowKeyDownThrough = false;
    void keyDown(enumKeyCodes key) override {
        if (key == enumKeyCodes::KEY_Escape || allowKeyDownThrough) {
            UILayer::keyDown(key);
            allowKeyDownThrough = false;
        }
    }
    void keyUp(enumKeyCodes key) override {
        if (allowKeyDownThrough) {
            UILayer::keyUp(key);
            allowKeyDownThrough = false;
        }
    }
};

$execute {
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/jump-p1",
        "Jump P1",
        "Player 1 Jump",
        { 
            Keybind::create(KEY_Space), 
            Keybind::create(KEY_W),
            ControllerBind::create(CONTROLLER_A),
            ControllerBind::create(CONTROLLER_Up),
            ControllerBind::create(CONTROLLER_RB) 
        },
        Category::PLAY,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/jump-p2",
        "Jump P2",
        "Player 2 Jump",
        { Keybind::create(KEY_Up), ControllerBind::create(CONTROLLER_LB) },
        Category::PLAY,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-left-p1",
        "Move left P1",
        "Moves P1 left in platformer mode",
        { Keybind::create(cocos2d::KEY_A), ControllerBind::create(CONTROLLER_Left), ControllerBind::create(CONTROLLER_LTHUMBSTICK_LEFT) },
        Category::PLAY,
        false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-right-p1",
        "Move right P1",
        "Moves P1 right in platformer mode",
        { Keybind::create(cocos2d::KEY_D), ControllerBind::create(CONTROLLER_Right), ControllerBind::create(CONTROLLER_LTHUMBSTICK_RIGHT) },
        Category::PLAY,
        false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-left-p2",
        "Move left P2",
        "Moves P2 left in platformer mode",
        { Keybind::create(KEY_Left), ControllerBind::create(CONTROLLER_RTHUMBSTICK_LEFT) },
        Category::PLAY,
        false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-right-p2",
        "Move right P2",
        "Moves P2 right in platformer mode",
        { Keybind::create(KEY_Right), ControllerBind::create(CONTROLLER_RTHUMBSTICK_RIGHT) },
        Category::PLAY,
        false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/place-checkpoint",
        "Place Checkpoint",
        "Place a Checkpoint in Practice Mode",
        { Keybind::create(KEY_Z, Modifier::None), ControllerBind::create(CONTROLLER_X) },
        Category::PLAY, false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/delete-checkpoint",
        "Delete Checkpoint",
        "Delete a Checkpoint in Practice Mode",
        { Keybind::create(KEY_X, Modifier::None), ControllerBind::create(CONTROLLER_B) },
        Category::PLAY, false
    });

    BindManager::get()->registerBindable({
        "robtop.geometry-dash/pause-level",
        "Pause Level",
        "Pause the Level",
        { ControllerBind::create(CONTROLLER_Start) },
        Category::PLAY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/toggle-hitboxes",
        "Toggle hitboxes",
        "Toggles hitboxes while in practice mode",
        { Keybind::create(KEY_P) },
        Category::PLAY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/restart-level",
        "Restart level",
        "Restarts the Level",
        { Keybind::create(cocos2d::KEY_R, Modifier::None) },
        Category::PLAY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/full-restart-level",
        "Full restart level",
        "Restarts the level from the beginning",
        { Keybind::create(KEY_R, Modifier::Control) },
        Category::PLAY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/practice-level",
        "Toggle Practice",
        "Toggles Practice Mode",
        { ControllerBind::create(CONTROLLER_X) },
        Category::PLAY_PAUSE, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/unpause-level",
        "Unpause Level",
        "Unpause the Level",
        { Keybind::create(KEY_Space, Modifier::None), ControllerBind::create(CONTROLLER_Start) },
        Category::PLAY_PAUSE, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/exit-level",
        "Exit Level",
        "Exit the Level",
        { ControllerBind::create(CONTROLLER_B) },
        Category::PLAY_PAUSE, false
    });
}
