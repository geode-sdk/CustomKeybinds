#include <Geode/modify/UILayer.hpp>
#include "../include/Keybinds.hpp"

using namespace geode::prelude;
using namespace keybinds;

struct $modify(UILayer) {
    static void onModify(auto& self) {
        (void)self.setHookPriority("UILayer::keyDown", 1000);
        (void)self.setHookPriority("UILayer::keyUp", 1000);
    }

    bool init() {
        if (!UILayer::init())
            return false;
        
        this->defineKeybind("robtop.geometry-dash/jump-p1", [=](bool down) {
            if (down) {
                PlayLayer::get()->pushButton(0, true);
            }
            else {
                PlayLayer::get()->releaseButton(0, true);
            }
        });
        this->defineKeybind("robtop.geometry-dash/jump-p2", [=](bool down) {
            if (down) {
                PlayLayer::get()->pushButton(0, false);
            }
            else {
                PlayLayer::get()->releaseButton(0, false);
            }
        });
        this->defineKeybind("robtop.geometry-dash/place-checkpoint", [=](bool down) {
            if (down && PlayLayer::get()->m_isPracticeMode) {
                this->onCheck(nullptr);
            }
        });
        this->defineKeybind("robtop.geometry-dash/delete-checkpoint", [=](bool down) {
            if (down && PlayLayer::get()->m_isPracticeMode) {
                this->onDeleteCheck(nullptr);
            }
        });
        
        return true;
    }

    void defineKeybind(const char* id, std::function<void(bool)> callback) {
        this->template addEventListener<InvokeBindFilter>([=](InvokeBindEvent* event) {
            callback(event->isDown());
            return ListenerResult::Propagate;
        }, id);
    }

    void keyDown(enumKeyCodes key) {
        if (key == enumKeyCodes::KEY_Escape) {
            UILayer::keyDown(key);
        }
    }
    void keyUp(enumKeyCodes) {}
};

$execute {
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/jump-p1",
        "Jump P1",
        "Player 1 Jump",
        { Keybind::create(KEY_Space, Modifier::None) },
        Category::PLAY,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/jump-p2",
        "Jump P2",
        "Player 2 Jump",
        { Keybind::create(KEY_Up, Modifier::None) },
        Category::PLAY,
        false
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/place-checkpoint",
        "Place Checkpoint",
        "Place a Checkpoint in Practice Mode",
        { Keybind::create(KEY_Z, Modifier::None) },
        Category::PLAY
    });
    BindManager::get()->registerBindable({
        "robtop.geometry-dash/delete-checkpoint",
        "Delete Checkpoint",
        "Delete a Checkpoint in Practice Mode",
        { Keybind::create(KEY_X, Modifier::None) },
        Category::PLAY
    });
}
