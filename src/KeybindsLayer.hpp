#pragma once

#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/InputNode.hpp>
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

class EditRepeatPopup : public Popup<BindableNode*>, public TextInputDelegate {
protected:
    BindableNode* m_node;
    RepeatOptions m_options;

    bool setup(BindableNode* node);

    void onEnabled(CCObject*);
    void textChanged(CCTextInputNode*) override;
    void onClose(CCObject*) override;

public:
    static EditRepeatPopup* create(BindableNode* node);
};

class BindableNode : public CCNode {
protected:
    BindableAction m_action;
    KeybindsLayer* m_layer;
    CCLayerColor* m_bg;
    CCMenu* m_bindMenu;
    CCMenu* m_nameMenu;
    bool m_expand = false;

    bool init(
        KeybindsLayer* layer,
        BindableAction const& action,
        float width, bool bgColor
    );
    void onEditBind(CCObject* sender);
    void onAddBind(CCObject*);
    void onExpand(CCObject*);
    void onResetToDefault(CCObject*);
    void onEditRepeat(CCObject*);
    void onInfo(CCObject*);

public:
    static BindableNode* create(
        KeybindsLayer* layer,
        BindableAction const& action,
        float width, bool bgColor
    );

    BindableAction const& getAction() const;
    std::string getMatchString() const;
    void updateMenu(bool updateLayer = true);
};

class KeybindsLayer : public Popup<>, public TextInputDelegate {
protected:
    ScrollLayer* m_scroll;
    std::vector<CCNode*> m_containers;
    std::vector<BindableNode*> m_nodes;
    InputNode* m_searchInput;
    CCLabelBMFont* m_resultsLabel;
    std::string m_query;

    bool setup() override;
    void textChanged(CCTextInputNode*) override;
    void onResetAll(CCObject*);

public:
    static KeybindsLayer* create();

    void search(std::string const& query);
    void updateAllBinds();
    void updateVisibility();
    void deselectSearchInput();
};
