#include "KeybindsLayer.hpp"

static ButtonSprite* createBindBtn(CCSprite* top) {
    auto spr = ButtonSprite::create(top, 18, true, 0, "square.png"_spr, 1.5f);
    spr->m_subBGSprite->setOpacity(85);
    spr->setScale(.45f);
    return spr;
}

static ButtonSprite* createBindBtn(const char* text) {
    auto spr = ButtonSprite::create(text, "goldFont.fnt", "square.png"_spr, .8f);
    spr->m_BGSprite->setOpacity(85);
    spr->setScale(.6f);
    return spr;
}

class FoldableLayer : public CCNode {
protected:
    ScrollLayer* m_scroll;
    bool m_folded = false;
    CCSprite* m_foldSpr;

    bool init(ScrollLayer* layer, std::string const& title, float width, size_t offset) {
        if (!CCNode::init())
            return false;
        
        m_scroll = layer;

        constexpr float height = 25.f;

        auto bg = CCLayerColor::create({ 0, 0, 0, 85 });
        bg->ignoreAnchorPointForPosition(false);
        bg->setAnchorPoint({ .0f, .0f });
        bg->setContentSize({ width, height });
        bg->setID("fold-layer");
        
        auto menu = CCMenu::create();
        menu->setContentSize({ height, height });
        menu->ignoreAnchorPointForPosition(false);
        menu->setPosition(width - height / 2, height / 2);
        menu->setAnchorPoint({ 1.f, .5f });

        m_foldSpr = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
        auto foldBtn = CCMenuItemSpriteExtra::create(
            createBindBtn(m_foldSpr), this, menu_selector(FoldableLayer::onFold)
        );
        foldBtn->setPosition(height / 2, height / 2);
        menu->addChild(foldBtn);
        bg->addChild(menu);

        if (offset > 0) {
            auto arrow = CCSprite::create("turn-arrow.png"_spr);
            arrow->setPosition({ height / 2 + 10.f, height / 2 });
            arrow->setOpacity(85);
            bg->addChild(arrow);
            limitNodeSize(arrow, { height - 10.f, height - 10.f }, 1.f, .1f);
        }
        auto titleLabel = CCLabelBMFont::create(title.c_str(), "bigFont.fnt");
        titleLabel->setOpacity(180);
        titleLabel->setColor({ 180, 180, 180 });
        titleLabel->setPosition(height / 2 + (offset ? 20.f : 0.f), height / 2);
        titleLabel->setAnchorPoint({ .0f, .5f });
        bg->addChild(titleLabel);
        limitNodeSize(titleLabel, { width - height * 2, height - 10.f }, 1.f, .1f);

        this->addChild(bg);

        this->setContentSize({ width, height });

        return true;
    }

    void onFold(CCObject*) {
        m_folded ^= 1;
        for (auto& child : CCArrayExt<CCNode>(m_pChildren)) {
            if (child->getID() != "fold-layer") {
                child->setVisible(!m_folded);
            }
        }
        m_foldSpr->setFlipY(m_folded);
        auto oldHeight = m_scroll->m_contentLayer->getContentSize().height;
        CCNode* ptr = this;
        do {
            ptr->setContentSize(ptr->getLayout()->getSizeHint(ptr));
            ptr->updateLayout();
        } while ((ptr = ptr->getParent()) && ptr->getLayout());
        m_scroll->scrollLayer(oldHeight - m_scroll->m_contentLayer->getContentSize().height);
    }

public:
    static FoldableLayer* create(ScrollLayer* layer, std::string const& title, float width, size_t offset) {
        auto ret = new FoldableLayer();
        if (ret && ret->init(layer, title, width, offset)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

bool EnterBindLayer::setup(BindableNode* node, Bind* original) {
    m_node = node;
    m_original = original;
    m_noElasticity = true;

    this->setTitle(original ? "Edit Keybind" : "Add Keybind");

    auto subTitle = CCLabelBMFont::create(node->m_action.getName().c_str(), "bigFont.fnt");
    subTitle->setColor(cc3x(0x3af));
    subTitle->setPosition(
        m_obContentSize.width / 2,
        m_obContentSize.height / 2 + m_size.height / 2 - 45.0f
    );
    subTitle->limitLabelWidth(m_size.width - 40.0f, .5f, .1f);
    m_mainLayer->addChild(subTitle);

    auto winSize = CCDirector::get()->getWinSize();

    m_label = CCLabelBMFont::create("Press Keys...", "bigFont.fnt");
    m_label->setOpacity(155);
    m_label->setPosition(winSize / 2);
    m_label->limitLabelWidth(m_size.width - 50.f, .7f, .1f);
    m_mainLayer->addChild(m_label);

    auto setSpr = ButtonSprite::create(original ? "Set" : "Add", "bigFont.fnt", "GJ_button_01.png", .8f);
    setSpr->setScale(.6f);
    auto setBtn = CCMenuItemSpriteExtra::create(
        setSpr, this, menu_selector(EnterBindLayer::onSet)
    );
    setBtn->setPosition(0.f, -m_size.height / 2 + 25.f);
    m_buttonMenu->addChild(setBtn);

    if (original) {
        auto remSpr = ButtonSprite::create("Remove", "bigFont.fnt", "GJ_button_06.png", .8f);
        remSpr->setScale(.6f);
        auto remBtn = CCMenuItemSpriteExtra::create(
            remSpr, this, menu_selector(EnterBindLayer::onRemove)
        );
        remBtn->setPosition(25.f, -m_size.height / 2 + 25.f);
        m_buttonMenu->addChild(remBtn);

        setBtn->setPositionX(-40.f);
    }

    return true;
}

void EnterBindLayer::onSet(CCObject*) {
    if (m_original) {
        BindManager::get()->removeBindFrom(m_node->m_action.getID(), m_original);
        m_node->updateMenu();
    }
    if (m_bind) {
        BindManager::get()->addBindTo(m_node->m_action.getID(), m_bind);
        m_node->updateMenu();
    }
    this->onClose(nullptr);
}

void EnterBindLayer::onRemove(CCObject*) {
    if (m_original) {
        BindManager::get()->removeBindFrom(m_node->m_action.getID(), m_original);
        m_node->updateMenu();
    }
    this->onClose(nullptr);
}

ListenerResult EnterBindLayer::onPressed(PressBindEvent* event) {
    if (event->isDown()) {
        m_label->setString(event->getBind()->toString().c_str());
        m_label->setOpacity(255);
        m_label->limitLabelWidth(m_size.width - 50.f, .7f, .1f);
        m_bind = event->getBind();
    }
    return ListenerResult::Stop;
}

EnterBindLayer* EnterBindLayer::create(BindableNode* node, Bind* original) {
    auto ret = new EnterBindLayer;
    if (ret && ret->init(220.f, 200.f, node, original)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool BindableNode::init(
    KeybindsLayer* layer,
    BindableAction const& action,
    float width, bool bgColor
) {
    if (!CCNode::init())
        return false;
    
    constexpr float height = 30.f;

    m_layer = layer;
    m_action = action;
    this->setContentSize({ width, height });

    auto bg = CCLayerColor::create({ 0, 0, 0, GLubyte(bgColor ? 120 : 70) });
    bg->ignoreAnchorPointForPosition(false);
    bg->setAnchorPoint({ 0.f, 0.f });
    bg->setContentSize({ width, height });
    this->addChild(bg);

    auto nameLabel = CCLabelBMFont::create(action.getName().c_str(), "bigFont.fnt");
    nameLabel->setPosition(height / 2, height / 2);
    nameLabel->setAnchorPoint({ .0f, .5f });
    this->addChild(nameLabel);
    limitNodeSize(nameLabel, { width / 2 - height, height / 1.33f }, .5f, .1f);

    m_menu = CCMenu::create();
    m_menu->setContentSize({ width / 2 - height, height });
    m_menu->ignoreAnchorPointForPosition(false);
    m_menu->setPosition(width - height / 2, height / 2);
    m_menu->setAnchorPoint({ 1.f, .5f });
    m_menu->setLayout(
        RowLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
    );
    this->addChild(m_menu);
    
    this->updateMenu();

    return true;
}

void BindableNode::updateMenu() {
    m_menu->removeAllChildren();
    for (auto& bind : BindManager::get()->getBindsFor(m_action.getID())) {
        auto spr = createBindBtn(bind->toString().c_str());
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(BindableNode::onEditBind)
        );
        btn->setUserObject(bind);
        m_menu->addChild(btn);
    }
    auto btn = CCMenuItemSpriteExtra::create(
        createBindBtn("+"),
        this, menu_selector(BindableNode::onAddBind)
    );
    m_menu->addChild(btn);
    m_menu->updateLayout();
}

void BindableNode::onAddBind(CCObject*) {
    EnterBindLayer::create(this)->show();
}

void BindableNode::onEditBind(CCObject* sender) {
    auto bind = static_cast<Bind*>(static_cast<CCNode*>(sender)->getUserObject());
    EnterBindLayer::create(this, bind)->show();
}

BindableNode* BindableNode::create(
    KeybindsLayer* layer,
    BindableAction const& action,
    float width, bool bgColor
) {
    auto ret = new BindableNode();
    if (ret && ret->init(layer, action, width, bgColor)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool KeybindsLayer::setup() {
    m_noElasticity = true;
    this->setTitle("Customize Keybinds");

    auto winSize = CCDirector::get()->getWinSize();
    auto scrollSize = CCSize { 340.f, 180.f };

    auto scroll = ScrollLayer::create(scrollSize);

    std::vector<CCNode*> targets { scroll->m_contentLayer };
    for (auto& category : BindManager::get()->getAllCategories()) {
        bool bgColor = false;
        CCNode* target = scroll->m_contentLayer;
        size_t offset = 0;
        for (auto& path : category.getPath()) {
            auto old = target;
            target = target->getChildByID(path);
            if (!target) {
                target = FoldableLayer::create(scroll, path, scrollSize.width, offset);
                target->setID(path);
                old->addChild(target);
                targets.insert(targets.begin(), target);
            }
            offset += 1;
        }
        for (auto& action : BindManager::get()->getBindables(category)) {
            auto node = BindableNode::create(this, action, scrollSize.width, bgColor ^= 1);
            target->addChild(node);
            m_nodes.push_back(node);
        }
    }

    for (auto& target : targets) {
        if (!target->getLayout()) {
            auto layout = ColumnLayout::create()
                ->setAxisReverse(true)
                ->setCrossAxisLineAlignment(AxisAlignment::End)
                ->setGap(0.f);
            layout->ignoreInvisibleChildren(true);
            target->setLayout(layout, false);
        }
        target->setContentSize(target->getLayout()->getSizeHint(target));
        target->updateLayout();
    }

    scroll->setPosition(winSize / 2 - scrollSize / 2 - ccp(0.f, 30.f));
    scroll->moveToTop();
    m_mainLayer->addChild(scroll);

    addListBorders(m_mainLayer, winSize / 2 - ccp(0.f, 30.f), scrollSize + ccp(4.f, 0.f));

    return true;
}

void KeybindsLayer::updateAll() {
    for (auto& node : m_nodes) {
        node->updateMenu();
    }
}

KeybindsLayer* KeybindsLayer::create() {
    auto ret = new KeybindsLayer;
    if (ret && ret->init(420.f, 280.f)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}
