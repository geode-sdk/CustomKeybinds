#pragma once

#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include "../include/Keybinds.hpp"

using namespace geode::prelude;
using namespace keybinds;

class KeybindsLayer;
class BindableNode;

class EnterBindLayer : public Popup<BindableNode*, Bind*> {
protected:
    BindableNode* m_node;
    CCLabelBMFont* m_label;
    Ref<Bind> m_bind;
    Ref<Bind> m_original;
    EventListener<PressBindFilter> m_listener = { this, &EnterBindLayer::onPressed };

    bool setup(BindableNode* node, Bind* modify);
    void onSet(CCObject*);
    void onRemove(CCObject*);
    ListenerResult onPressed(PressBindEvent* event);

public:
    static EnterBindLayer* create(BindableNode* node, Bind* modify = nullptr);
};

class BindableNode : public CCNode {
protected:
    BindableAction m_action;
    KeybindsLayer* m_layer;
    CCMenu* m_menu;

    bool init(
        KeybindsLayer* layer,
        BindableAction const& action,
        float width, bool bgColor
    );
    void onEditBind(CCObject* sender);
    void onAddBind(CCObject*);

    friend class EnterBindLayer;

public:
    static BindableNode* create(
        KeybindsLayer* layer,
        BindableAction const& action,
        float width, bool bgColor
    );

    void updateMenu();
};

class KeybindsLayer : public Popup<> {
protected:
    std::vector<BindableNode*> m_nodes;

    bool setup() override;

public:
    static KeybindsLayer* create();

    void updateAll();
};
