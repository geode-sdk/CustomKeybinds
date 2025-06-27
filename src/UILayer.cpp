#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/binding/PauseLayer.hpp>
#include "Geode/binding/UILayer.hpp"
#include <Geode/cocos/CCDirector.h>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCScene.h>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>

#include "../include/Keybinds.hpp"

using namespace geode::prelude;
using namespace keybinds;

static void addBindSprites(CCNodeRGBA* target, const char* action) {
    if (target == nullptr) return;
    target->removeAllChildren();

    auto bindContainer = CCNode::create();
    bool first = true;
    for (auto& bind : BindManager::get()->getBindsFor(action)) {
        if (!first) {
            auto separator = CCLabelBMFont::create("/", "bigFont.fnt");
            separator->setScale(.8f);
            separator->setOpacity(target->getOpacity());
            bindContainer->addChild(separator);
        }
        first = false;
        auto label = bind->createLabel();
        if (auto text = typeinfo_cast<CCLabelBMFont*>(label)) {
            text->setFntFile("bigFont.fnt");
        }
        label->setScale(.8f);
        if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(label)) {
            rgba->setOpacity(target->getOpacity());
        }
        bindContainer->addChild(label);
    }
    bindContainer->setID("binds"_spr);
    bindContainer->setContentSize({
        target->getScaledContentWidth(), 40.f
    });
    bindContainer->setLayout(SimpleRowLayout::create()->setMainAxisScaling(AxisScaling::ScaleDown));
    bindContainer->setAnchorPoint({ .5f, .5f });
    bindContainer->setPosition(target->getContentWidth() / 2, -1.f);
    target->addChild(bindContainer);
}

struct $modify(PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!event->isDown()) {
                return ListenerResult::Propagate;
            }

            // Remove any popups (looking at you, confirm exit)
            CCScene* active = CCDirector::sharedDirector()->getRunningScene();
            if (auto alert = active->getChildByType<FLAlertLayer>(0)) {
                return ListenerResult::Propagate;
            }
            this->onResume(nullptr);

            return ListenerResult::Stop;
        }, "robtop.geometry-dash/unpause-level");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (event->isDown()) {
                this->onQuit(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        }, "robtop.geometry-dash/exit-level");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
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
    }

    void keyDown(enumKeyCodes key) {
        if (key == enumKeyCodes::KEY_Escape) {
            PauseLayer::keyDown(key);
        }
    }
};

struct $modify(UILayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("UILayer::handleKeypress", 1000);
    }

    static inline int platformButton() {
        return 1;
    }

    bool isPaused() {
        return !this->isCurrentPlayLayer()
            || PlayLayer::get()->getParent()->getChildByType<PauseLayer>(0) != nullptr
            || PlayLayer::get()->getChildByType<GJDropDownLayer>(0) != nullptr;
    }

    bool isCurrentPlayLayer() {
        auto playLayer = CCScene::get()->getChildByType<PlayLayer>(0);
        return playLayer != nullptr && playLayer == PlayLayer::get() && playLayer->getChildByType<UILayer>(0) == this;
    }

    void pressKeyFallthrough(enumKeyCodes key, bool down) {
        auto dispatcher = CCDirector::get()->getKeyboardDispatcher();

        auto shift = dispatcher->getShiftKeyPressed();
        auto ctrl = dispatcher->getControlKeyPressed();
        auto alt = dispatcher->getAltKeyPressed();
        auto cmd = dispatcher->getCommandKeyPressed();

        pressKeyFallthrough(key, down, shift, ctrl, alt, cmd);
    }

    void pressKeyFallthrough(enumKeyCodes key, bool down, bool shift, bool ctrl, bool alt, bool cmd) {
        // amazing hack
        if (this->isPaused())
            return;

        // enable quick keys - this mildly diverts from vanilla behavior
        // because some keybinds are only supposed to work with that option enabled
        // however we do provide our own way to disable them
        // so requiring them both to be enabled just ended up confusing users
        auto GM = GameManager::sharedState();
        auto quickKeys = GM->getGameVariable("0163");
        GM->setGameVariable("0163", true);

        auto dispatcher = CCDirector::get()->getKeyboardDispatcher();

        auto oShift = dispatcher->getShiftKeyPressed();
        auto oCtrl = dispatcher->getControlKeyPressed();
        auto oAlt = dispatcher->getAltKeyPressed();
        auto oCmd = dispatcher->getCommandKeyPressed();

        dispatcher->updateModifierKeys(shift, ctrl, alt, cmd);

        allowKeyDownThrough = true;
        if (down) {
            this->keyDown(key);
        } else {
            this->keyUp(key);
        }
        allowKeyDownThrough = false;

        dispatcher->updateModifierKeys(oShift, oCtrl, oAlt, oCmd);

        //disable quick keys to restore previous state
        GM->setGameVariable("0163", quickKeys);
    }

    bool init(GJBaseGameLayer* layer) {
        if (!UILayer::init(layer))
            return false;

        // delay by a single frame
        geode::Loader::get()->queueInMainThread([this] {
            // do not do anything in the editor
            if (!PlayLayer::get()) return;

            this->defineKeybind("robtop.geometry-dash/jump-p1", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                PlayLayer::get()->queueButton(1, down, false);
                if (down) {
                    m_p1Jumping = true;
                } else {
                    m_p1Jumping = false;
                }
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/jump-p2", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                PlayLayer::get()->queueButton(1, down, true);
                if (down) {
                    m_p2Jumping = true;
                } else {
                    m_p2Jumping = false;
                }
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/place-checkpoint", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_Z, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/delete-checkpoint", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_X, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/pause-level", [this](bool down) {
                if (down && this->isCurrentPlayLayer() && !this->isPaused()) {
                    PlayLayer::get()->pauseGame(true);
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("robtop.geometry-dash/restart-level", [this](bool down) {
                if (this->isPaused() || !this->isCurrentPlayLayer()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_R, down, false, false, false, false);
                return ListenerResult::Propagate;
            });
            this->defineKeybind("robtop.geometry-dash/full-restart-level", [this](bool down) {
                if (this->isPaused() || !this->isCurrentPlayLayer()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_R, down, false, true, false, false);
                return ListenerResult::Propagate;
            });
            this->defineKeybind("robtop.geometry-dash/move-left-p1", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_A, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/move-right-p1", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_D, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/move-left-p2", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_Left, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/move-right-p2", [this](bool down) {
                if (this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_Right, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("robtop.geometry-dash/toggle-hitboxes", [this](bool down) {
                if (down && this->isCurrentPlayLayer() && !this->isPaused()) {
                    // This assumes you have quick keys on
                    this->pressKeyFallthrough(KEY_P, down);
                }
                return ListenerResult::Stop;
            });
            // display practice mode button keybinds
            if (auto menu = this->getChildByID("checkpoint-menu")) {
                if (auto add = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("add-checkpoint-button"))) {
                    addBindSprites(
                        typeinfo_cast<CCNodeRGBA*>(add->getNormalImage()),
                        "robtop.geometry-dash/place-checkpoint"
                    );
                }
                if (auto rem = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("remove-checkpoint-button"))) {
                    addBindSprites(
                        typeinfo_cast<CCNodeRGBA*>(rem->getNormalImage()),
                        "robtop.geometry-dash/delete-checkpoint"
                    );
                }
            }
        });

        return true;
    }

    void defineKeybind(const char* id, std::function<ListenerResult(bool)> callback) {
        // adding the events to playlayer instead
        PlayLayer::get()->template addEventListener<InvokeBindFilter>([this, callback](InvokeBindEvent* event) {
            return callback(event->isDown());
        }, id);
    }

    static inline bool allowKeyDownThrough = false;
    void handleKeypress(cocos2d::enumKeyCodes key, bool down) {
        if (key == enumKeyCodes::KEY_Escape || allowKeyDownThrough) {
            UILayer::handleKeypress(key, down);
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
        {
            Keybind::create(KEY_Up),
            GEODE_MACOS(Keybind::create(KEY_ArrowUp, Modifier::None),)
            ControllerBind::create(CONTROLLER_LB)
        },
        Category::PLAY,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-left-p1",
        "Move left P1",
        "Player 1 move left",
        {
            Keybind::create(cocos2d::KEY_A),
            ControllerBind::create(CONTROLLER_Left),
            ControllerBind::create(CONTROLLER_LTHUMBSTICK_LEFT)
        },
        Category::PLAY,
        false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-right-p1",
        "Move right P1",
        "Player 1 move right",
        {
            Keybind::create(cocos2d::KEY_D),
            ControllerBind::create(CONTROLLER_Right),
            ControllerBind::create(CONTROLLER_LTHUMBSTICK_RIGHT)
        },
        Category::PLAY,
        false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-left-p2",
        "Move left P2",
        "Player 2 move left",
        {
            Keybind::create(KEY_Left),
            GEODE_MACOS(Keybind::create(KEY_ArrowLeft, Modifier::None),)
            ControllerBind::create(CONTROLLER_RTHUMBSTICK_LEFT)
        },
        Category::PLAY,
        false 
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/move-right-p2",
        "Move right P2",
        "Player 2 move right",
        {
            Keybind::create(KEY_Right),
            GEODE_MACOS(Keybind::create(KEY_ArrowRight, Modifier::None),)
            ControllerBind::create(CONTROLLER_RTHUMBSTICK_RIGHT)
        },
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
        "Restarts the Level (needs Quick Keys enabled)",
        { Keybind::create(cocos2d::KEY_R, Modifier::None) },
        Category::PLAY, false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/full-restart-level",
        "Full restart level",
        "Restarts the level from the beginning (needs Quick Keys enabled)",
        { Keybind::create(KEY_R, Modifier::Control), GEODE_MACOS(Keybind::create(KEY_R, Modifier::Command)) },
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
