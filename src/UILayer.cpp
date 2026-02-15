#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GJDropDownLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/SimpleAxisLayout.hpp>
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

static void addBindSprites(CCNodeRGBA* target, const char* action) {
    if (target == nullptr) return;
    target->removeAllChildren();

    auto bindContainer = CCNode::create();
    bool first = true;
    for (auto& bind : Mod::get()->getSettingValue<std::vector<Keybind>>(action)) {
        if (!first) {
            auto separator = CCLabelBMFont::create("/", "bigFont.fnt");
            separator->setScale(.8f);
            separator->setOpacity(target->getOpacity());
            bindContainer->addChild(separator);
        }
        first = false;
        auto label = bind.createNode();
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

        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "unpause-level"), [this](Keybind const& keybind, bool down, bool repeat) {
            if (repeat || !down) {
                return ListenerResult::Propagate;
            }

            // Remove any popups (looking at you, confirm exit)
            CCScene* active = CCDirector::sharedDirector()->getRunningScene();
            if (auto alert = active->getChildByType<FLAlertLayer>(0)) {
                return ListenerResult::Propagate;
            }
            this->onResume(nullptr);

            return ListenerResult::Stop;
        });

        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "exit-level"), [this](Keybind const& keybind, bool down, bool repeat) {
            if (!repeat && down) {
                this->onQuit(nullptr);
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        });

        this->addEventListener(KeybindSettingPressedEventV3(Mod::get(), "practice-level"), [this](Keybind const& keybind, bool down, bool repeat) {
            if (!repeat && down) {
                if(PlayLayer::get() && PlayLayer::get()->m_isPracticeMode) {
                    this->onNormalMode(nullptr);
                } else {
                    this->onPracticeMode(nullptr);
                }
                return ListenerResult::Stop;
            }
            return ListenerResult::Propagate;
        });
    }

    void keyDown(enumKeyCodes key, double timestamp) {
        if (key == enumKeyCodes::KEY_Escape) {
            PauseLayer::keyDown(key, timestamp);
        }
    }
};

struct $modify(UILayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("UILayer::handleKeypress", Priority::Late);
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
            this->keyDown(key, getInputTimestamp());
        } else {
            this->keyUp(key, getInputTimestamp());
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

            this->defineKeybind("jump-p1", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                PlayLayer::get()->queueButton(1, down, false, getInputTimestamp());
                if (down) {
                    m_p1Jumping = true;
                } else {
                    m_p1Jumping = false;
                }
                return ListenerResult::Stop;
            });
            this->defineKeybind("jump-p2", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                PlayLayer::get()->queueButton(1, down, true, getInputTimestamp());
                if (down) {
                    m_p2Jumping = true;
                } else {
                    m_p2Jumping = false;
                }
                return ListenerResult::Stop;
            });
            this->defineKeybind("place-checkpoint", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_Z, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("delete-checkpoint", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_X, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("pause-level", [this](bool down, bool repeat) {
                if (!repeat && down && this->isCurrentPlayLayer() && !this->isPaused()) {
                    PlayLayer::get()->pauseGame(true);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            this->defineKeybind("restart-level", [this](bool down, bool repeat) {
                if (repeat || this->isPaused() || !this->isCurrentPlayLayer()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_R, down, false, false, false, false);
                return ListenerResult::Stop;
            });
            this->defineKeybind("full-restart-level", [this](bool down, bool repeat) {
                if (repeat || this->isPaused() || !this->isCurrentPlayLayer()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_R, down, false, true, false, false);
                return ListenerResult::Stop;
            });
            this->defineKeybind("move-left-p1", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_A, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("move-right-p1", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_D, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("move-left-p2", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_Left, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("move-right-p2", [this](bool down, bool repeat) {
                if (repeat || this->isPaused()) {
                    return ListenerResult::Propagate;
                }
                this->pressKeyFallthrough(KEY_Right, down);
                return ListenerResult::Stop;
            });
            this->defineKeybind("toggle-hitboxes", [this](bool down, bool repeat) {
                if (!repeat && down && this->isCurrentPlayLayer() && !this->isPaused()) {
                    // This assumes you have quick keys on
                    this->pressKeyFallthrough(KEY_P, down);
                    return ListenerResult::Stop;
                }
                return ListenerResult::Propagate;
            });
            // display practice mode button keybinds
            if (auto menu = this->getChildByID("checkpoint-menu")) {
                if (auto add = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("add-checkpoint-button"))) {
                    addBindSprites(
                        typeinfo_cast<CCNodeRGBA*>(add->getNormalImage()),
                        "place-checkpoint"
                    );
                }
                if (auto rem = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("remove-checkpoint-button"))) {
                    addBindSprites(
                        typeinfo_cast<CCNodeRGBA*>(rem->getNormalImage()),
                        "delete-checkpoint"
                    );
                }
            }
        });

        return true;
    }

    void defineKeybind(std::string id, CopyableFunction<bool(bool, bool)> callback) {
        // adding the events to playlayer instead
        PlayLayer::get()->addEventListener(KeybindSettingPressedEventV3(Mod::get(), std::move(id)), [callback = std::move(callback)](Keybind const& keybind, bool down, bool repeat) {
            return callback(down, repeat);
        });
    }

    static inline bool allowKeyDownThrough = false;
    void handleKeypress(cocos2d::enumKeyCodes key, bool down, double timestamp) {
        if (key == enumKeyCodes::KEY_Escape || allowKeyDownThrough) {
            UILayer::handleKeypress(key, down, timestamp);
            allowKeyDownThrough = false;
        }
    }
};
