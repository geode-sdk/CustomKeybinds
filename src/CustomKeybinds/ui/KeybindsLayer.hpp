#pragma once

#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/binding/TextInputDelegate.hpp>

#include <Keybinds.hpp>

using namespace geode::prelude;
using namespace geode::keybinds;

class KeybindsLayer;

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
        float width, size_t offset, bool bgColor
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
        float width, size_t offset, bool bgColor
    );

    BindableAction const& getAction() const;
    std::string getMatchString() const;
    void updateMenu(bool updateLayer = true);
};

class EnterBindLayer : public Popup<BindableNode*, Bind*> {
protected:
    Ref<BindableNode> m_node;
    CCNode* m_label;
    Ref<Bind> m_bind;
    Ref<Bind> m_original;
    CCLabelBMFont* m_usedByLabel;
    EventListener<PressBindFilter> m_listener = { this, &EnterBindLayer::onPressed };

    bool setup(BindableNode* node, Bind* modify);
    void updateUsedBy();
    void onSet(CCObject*);
    void onRemove(CCObject*);
    ListenerResult onPressed(PressBindEvent* event);

public:
    static EnterBindLayer* create(BindableNode* node, Bind* modify = nullptr);
};

class EditRepeatPopup : public Popup<BindableNode*> {
protected:
    Ref<BindableNode> m_node;
    RepeatOptions m_options;

    bool setup(BindableNode* node) override;

    void onEnabled(CCObject*);
    void onClose(CCObject*) override;

public:
    static EditRepeatPopup* create(BindableNode* node);
};

class KeybindsLayer : public Popup<>, public TextInputDelegate {
protected:
    ScrollLayer* m_scroll;
    std::vector<CCNode*> m_containers;
    std::vector<BindableNode*> m_nodes;
    TextInput* m_searchInput;
    CCLabelBMFont* m_resultsLabel;
    std::string m_query;
    EventListener<DeviceFilter> m_deviceListener = 
        EventListener<DeviceFilter>(this, &KeybindsLayer::onDevice);

    bool setup() override;
    void textChanged(CCTextInputNode*) override;
    void onResetAll(CCObject*);
    void onDevice(DeviceEvent*);

public:
    static KeybindsLayer* create();

    void search(std::string const& query);
    void updateAllBinds();
    void updateVisibility();
    void deselectSearchInput();
};
