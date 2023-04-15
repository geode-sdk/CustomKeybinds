#include "KeybindsLayer.hpp"
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/ui/General.hpp>
#include <Geode/ui/Scrollbar.hpp>

static ButtonSprite* createBindBtn(CCSprite* top) {
    auto spr = ButtonSprite::create(top, 18, true, 0, "square.png"_spr, 2.f);
    spr->m_subBGSprite->setOpacity(85);
    spr->m_subSprite->setScale(1.f);
    spr->m_subSprite->setPosition(spr->getContentSize() / 2);
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
    KeybindsLayer* m_layer;
    bool m_folded = false;
    CCSprite* m_foldSpr;

    bool init(KeybindsLayer* layer, std::string const& title, float width, size_t offset) {
        if (!CCNode::init())
            return false;
        
        m_layer = layer;

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
        m_layer->updateVisibility();
    }

public:
    static FoldableLayer* create(KeybindsLayer* layer, std::string const& title, float width, size_t offset) {
        auto ret = new FoldableLayer();
        if (ret && ret->init(layer, title, width, offset)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool isFolded() const {
        return m_folded;
    }
};

bool EnterBindLayer::setup(BindableNode* node, Bind* original) {
    m_node = node;
    m_original = original;
    m_noElasticity = true;

    this->setTitle(original ? "Edit Keybind" : "Add Keybind");

    auto subTitle = CCLabelBMFont::create(node->getAction().getName().c_str(), "bigFont.fnt");
    subTitle->setColor({ 51, 170, 255 });
    subTitle->setPosition(
        m_obContentSize.width / 2,
        m_obContentSize.height / 2 + m_size.height / 2 - 45.0f
    );
    subTitle->limitLabelWidth(m_size.width - 40.0f, .5f, .1f);
    m_mainLayer->addChild(subTitle);

    auto winSize = CCDirector::get()->getWinSize();

    auto label = CCLabelBMFont::create("Press Keys...", "bigFont.fnt");
    label->setOpacity(155);
    label->setPosition(winSize / 2);
    label->limitLabelWidth(m_size.width - 50.f, .7f, .1f);
    m_mainLayer->addChild(m_label = label);

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

        auto originalLabel = original->createBindSprite();
        originalLabel->setScale(.4f);
        originalLabel->setPosition(m_obContentSize/ 2 - ccp(0, 45.f));
        m_mainLayer->addChild(originalLabel);
    }

    return true;
}

void EnterBindLayer::onSet(CCObject*) {
    if (m_original) {
        BindManager::get()->removeBindFrom(m_node->getAction().getID(), m_original);
        m_node->updateMenu();
    }
    if (m_bind) {
        BindManager::get()->addBindTo(m_node->getAction().getID(), m_bind);
        m_node->updateMenu();
    }
    this->onClose(nullptr);
}

void EnterBindLayer::onRemove(CCObject*) {
    if (m_original) {
        BindManager::get()->removeBindFrom(m_node->getAction().getID(), m_original);
        m_node->updateMenu();
    }
    this->onClose(nullptr);
}

ListenerResult EnterBindLayer::onPressed(PressBindEvent* event) {
    if (event->isDown()) {
        auto old = m_label;
        m_label = event->getBind()->createLabel();
        limitNodeSize(
            m_label,
            { m_size.width - 50.f, m_size.height - 100.f },
            1.f, .1f
        );
        m_label->setPosition(old->getPosition());
        old->getParent()->addChild(m_label);
        old->removeFromParent();
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

bool EditRepeatPopup::setup(BindableNode* node) {
    m_noElasticity = true;
    m_node = node;
    this->setTitle("Repeat Options");

    m_options = BindManager::get()->getRepeatOptionsFor(node->getAction().getID())
        .value_or(RepeatOptions());

    auto winSize = CCDirector::get()->getWinSize();

    auto subTitle = CCLabelBMFont::create(node->getAction().getName().c_str(), "bigFont.fnt");
    subTitle->setColor({ 51, 170, 255 });
    subTitle->setPosition(
        m_obContentSize.width / 2,
        m_obContentSize.height / 2 + m_size.height / 2 - 45.0f
    );
    subTitle->limitLabelWidth(m_size.width - 40.0f, .5f, .1f);
    m_mainLayer->addChild(subTitle);

    auto rateLabel = CCLabelBMFont::create("Rate", "bigFont.fnt");
    rateLabel->setScale(.5f);
    rateLabel->setAnchorPoint({ 1.f, .5f });
    rateLabel->setPosition(m_obContentSize / 2 + ccp(-15.f, 20.f));
    m_mainLayer->addChild(rateLabel);

    auto rateInput = InputNode::create(80.f, "ms");
    rateInput->setPosition(m_obContentSize / 2 + ccp(35.f, 20.f));
    rateInput->getInput()->setString(std::to_string(m_options.rate));
    rateInput->getInput()->setDelegate(this);
    rateInput->getInput()->setUserData(&m_options.rate);
    rateInput->setScale(.75f);
    m_mainLayer->addChild(rateInput);

    auto delayLabel = CCLabelBMFont::create("Delay", "bigFont.fnt");
    delayLabel->setScale(.5f);
    delayLabel->setAnchorPoint({ 1.f, .5f });
    delayLabel->setPosition(m_obContentSize / 2 + ccp(-15.f, -10.f));
    m_mainLayer->addChild(delayLabel);

    auto delayInput = InputNode::create(80.f, "ms");
    delayInput->setPosition(m_obContentSize / 2 + ccp(35.f, -10.f));
    delayInput->getInput()->setString(std::to_string(m_options.delay));
    delayInput->getInput()->setDelegate(this);
    delayInput->getInput()->setUserData(&m_options.delay);
    delayInput->setScale(.75f);
    m_mainLayer->addChild(delayInput);

    auto toggleLabel = CCLabelBMFont::create("Enable", "bigFont.fnt");
    toggleLabel->setScale(.5f);
    toggleLabel->setAnchorPoint({ .0f, .5f });
    toggleLabel->setPosition(m_obContentSize / 2 + ccp(-15.f, -40.f));
    m_mainLayer->addChild(toggleLabel);

    auto toggle = CCMenuItemToggler::createWithStandardSprites(
        this, menu_selector(EditRepeatPopup::onEnabled), .65f
    );
    toggle->setPosition(-35.f, -40.f);
    toggle->toggle(m_options.enabled);
    m_buttonMenu->addChild(toggle);

    auto okSpr = ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", .8f);
    okSpr->setScale(.8f);
    auto okBtn = CCMenuItemSpriteExtra::create(
        okSpr, this, menu_selector(EditRepeatPopup::onClose)
    );
    okBtn->setPosition(0.f, -m_size.height / 2 + 25.f);
    m_buttonMenu->addChild(okBtn);

    return true;
}

void EditRepeatPopup::textChanged(CCTextInputNode* input) {
    try {
        *static_cast<size_t*>(input->getUserData()) = std::stoi(input->getString());
    }
    catch(...) {}
}

void EditRepeatPopup::onEnabled(CCObject* sender) {
    m_options.enabled = !static_cast<CCMenuItemToggler*>(sender)->isToggled();
}

void EditRepeatPopup::onClose(CCObject*) {
    BindManager::get()->setRepeatOptionsFor(m_node->getAction().getID(), m_options);
    m_node->updateMenu();
    Popup::onClose(nullptr);
}

EditRepeatPopup* EditRepeatPopup::create(BindableNode* node) {
    auto ret = new EditRepeatPopup;
    if (ret && ret->init(220.f, 200.f, node)) {
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

    m_bg = CCLayerColor::create({ 0, 0, 0, GLubyte(bgColor ? 120 : 70) });
    m_bg->ignoreAnchorPointForPosition(false);
    m_bg->setAnchorPoint({ 0.f, 0.f });
    m_bg->setContentSize({ width, height });
    this->addChild(m_bg);

    m_nameMenu = CCMenu::create();
    m_nameMenu->setContentSize({ width / 2, height });
    m_nameMenu->setAnchorPoint({ .0f, .5f });
    m_nameMenu->setPosition(height / 2, height / 2);
    m_nameMenu->ignoreAnchorPointForPosition(false);

    auto nameLabel = CCLabelBMFont::create(action.getName().c_str(), "bigFont.fnt");
    nameLabel->setPosition(0.f, height / 2);
    nameLabel->setAnchorPoint({ .0f, .5f });
    m_nameMenu->addChild(nameLabel);
    limitNodeSize(nameLabel, { width / 2 - height, height / 1.33f }, .5f, .1f);

    if (action.getMod() != Mod::get()) {
        auto modLabel = CCLabelBMFont::create(action.getMod()->getName().c_str(), "bigFont.fnt");
        modLabel->setPosition(0.f, height / 2 - 7.f);
        modLabel->setAnchorPoint({ .0f, .5f });
        modLabel->setScale(.25f);
        modLabel->setColor({ 125, 183, 230 });
        m_nameMenu->addChild(modLabel);

        nameLabel->setPositionY(height / 2 + 5.5f);
    }

    if (action.getDescription().size()) {
        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(.55f);
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr, this, menu_selector(BindableNode::onInfo)
        );
        infoBtn->setPosition(nameLabel->boundingBox().getMaxX() + 15.f, height / 2);
        m_nameMenu->addChild(infoBtn);
    }

    this->addChild(m_nameMenu);

    m_bindMenu = CCMenu::create();
    m_bindMenu->setContentSize({ width / 2 - height, height });
    m_bindMenu->ignoreAnchorPointForPosition(false);
    m_bindMenu->setPosition(width - height / 2, height / 2);
    m_bindMenu->setAnchorPoint({ 1.f, .5f });
    m_bindMenu->setLayout(
        RowLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
    );
    this->addChild(m_bindMenu);

    this->updateMenu(false);

    return true;
}

void BindableNode::updateMenu(bool updateLayer) {
    static_cast<AxisLayout*>(m_bindMenu->getLayout())->setGrowCrossAxis(m_expand);
    m_bindMenu->removeAllChildren();
    auto binds = BindManager::get()->getBindsFor(m_action.getID());
    size_t i = 0;
    float length = 0.f;
    for (auto& bind : binds) {
        auto spr = bind->createBindSprite();
        length += spr->getScaledContentSize().width;
        if (length > m_obContentSize.width / 2) {
            if (!m_expand) break;
        }
        i++;
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(BindableNode::onEditBind)
        );
        btn->setUserObject(bind);
        m_bindMenu->addChild(btn);
    }
    if (i != binds.size()) {
        m_bindMenu->addChild(CCMenuItemSpriteExtra::create(
            createBindBtn("..."),
            this, menu_selector(BindableNode::onExpand)
        ));
    }
    else {
        m_bindMenu->addChild(CCMenuItemSpriteExtra::create(
            createBindBtn("+"),
            this, menu_selector(BindableNode::onAddBind)
        ));
        if (auto options = BindManager::get()->getRepeatOptionsFor(m_action.getID())) {
            auto clock = CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
            if (!options.value().enabled) {
                clock->setOpacity(165);
                clock->setColor({ 165, 165, 165 });
            }
            m_bindMenu->addChild(CCMenuItemSpriteExtra::create(
                createBindBtn(clock),
                this, menu_selector(BindableNode::onEditRepeat)
            ));
        }
        if (!BindManager::get()->hasDefaultBinds(m_action.getID())) {
            m_bindMenu->addChild(CCMenuItemSpriteExtra::create(
                createBindBtn(CCSprite::createWithSpriteFrameName("edit_ccwBtn_001.png")),
                this, menu_selector(BindableNode::onResetToDefault)
            ));
        }
    }
    m_bindMenu->updateLayout();

    auto oldHeight = m_obContentSize.height;
    this->setContentSize({
        m_obContentSize.width,
        m_bindMenu->getScaledContentSize().height + 10.f
    });
    m_bg->setContentSize(m_obContentSize);
    m_nameMenu->setPositionY(m_obContentSize.height / 2);
    m_bindMenu->setPositionY(m_obContentSize.height / 2);
    if (updateLayer && oldHeight != m_obContentSize.height) {
        m_layer->updateVisibility();
    }
}

void BindableNode::onAddBind(CCObject*) {
    m_layer->deselectSearchInput();
    m_expand = true;
    EnterBindLayer::create(this)->show();
}

void BindableNode::onEditBind(CCObject* sender) {
    m_layer->deselectSearchInput();
    auto bind = static_cast<Bind*>(static_cast<CCNode*>(sender)->getUserObject());
    EnterBindLayer::create(this, bind)->show();
}

void BindableNode::onResetToDefault(CCObject*) {
    m_layer->deselectSearchInput();
    createQuickPopup(
        "Reset Bind",
        fmt::format(
            "Are you sure you want to <cr>reset</c> <cb>{}</c> to default?",
            m_action.getName()
        ),
        "Cancel", "Reset",
        [=](auto*, bool btn2) {
            if (btn2) {
                BindManager::get()->resetBindsToDefault(m_action.getID());
                this->updateMenu();
            }
        }
    );
}

void BindableNode::onEditRepeat(CCObject*) {
    m_layer->deselectSearchInput();
    EditRepeatPopup::create(this)->show();
}

void BindableNode::onExpand(CCObject*) {
    m_expand = true;
    this->updateMenu();
}

void BindableNode::onInfo(CCObject*) {
    FLAlertLayer::create(
        m_action.getName().c_str(),
        fmt::format(
            "{}\n\nAdded by <cp>{}</c>",
            m_action.getDescription(), m_action.getMod()->getID()
        ),
        "OK"
    )->show();
}

BindableAction const& BindableNode::getAction() const {
    return m_action;
}

std::string BindableNode::getMatchString() const {
    auto res = m_action.getMod()->getName() + " " + 
        m_action.getCategory().toString() + " " +
        m_action.getName() + " ";
    for (auto& bind : BindManager::get()->getBindsFor(m_action.getID())) {
        res += bind->toString() + ", ";
    }
    return res;
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

    m_searchInput = InputNode::create(scrollSize.width / .8f, "Search Bindings...");
    m_searchInput->setPosition(winSize.width / 2, winSize.height / 2 + scrollSize.height / 2);
    m_searchInput->getInput()->getPlaceholderLabel()->setAnchorPoint({ .0f, .5f });
    m_searchInput->getInput()->updateLabel(""); // trigger position fix from Geode
    m_searchInput->getInput()->setPositionX(
        m_searchInput->getInput()->getPositionX() - scrollSize.width / 2 / .8f + 10.f
    );
    m_searchInput->getInput()->setLabelPlaceholderScale(.5f);
    m_searchInput->getInput()->setMaxLabelScale(.5f);
    m_searchInput->getInput()->setDelegate(this);
    m_searchInput->setScale(.8f);
    m_mainLayer->addChild(m_searchInput);

    m_resultsLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_resultsLabel->setScale(.4f);
    m_resultsLabel->setAnchorPoint({ .0f, .5f });
    m_resultsLabel->setPosition(
        winSize.width / 2 - scrollSize.width / 2,
        winSize.height / 2 + scrollSize.height / 2 - 20.f
    );
    m_resultsLabel->setZOrder(5);
    m_mainLayer->addChild(m_resultsLabel);

    m_scroll = ScrollLayer::create(scrollSize);

    m_containers = { m_scroll->m_contentLayer };
    for (auto& category : BindManager::get()->getAllCategories()) {
        bool bgColor = false;
        CCNode* target = m_scroll->m_contentLayer;
        size_t offset = 0;
        for (auto& path : category.getPath()) {
            auto old = target;
            target = target->getChildByID(path);
            if (!target) {
                target = FoldableLayer::create(this, path, scrollSize.width, offset);
                target->setID(path);
                old->addChild(target);
                m_containers.insert(m_containers.begin(), target);
            }
            offset += 1;
        }
        for (auto& action : BindManager::get()->getBindablesIn(category)) {
            auto node = BindableNode::create(this, action, scrollSize.width, bgColor ^= 1);
            target->addChild(node);
            m_nodes.push_back(node);
        }
    }

    for (auto& target : m_containers) {
        auto layout = ColumnLayout::create()
            ->setAxisReverse(true)
            ->setCrossAxisLineAlignment(AxisAlignment::End)
            ->setAxisAlignment(AxisAlignment::End)
            ->setGap(0.f);
        layout->ignoreInvisibleChildren(true);
        target->setLayout(layout, false);
    }

    m_scroll->setPosition(winSize / 2 - scrollSize / 2 - ccp(0.f, 30.f));
    m_scroll->moveToTop();
    m_mainLayer->addChild(m_scroll);

    addListBorders(m_mainLayer, winSize / 2 - ccp(0.f, 30.f), scrollSize + ccp(4.f, 0.f));

    auto bar = Scrollbar::create(m_scroll);
    bar->setPosition(winSize / 2 + ccp(scrollSize.width / 2 + 20.f, -30.f));
    m_mainLayer->addChild(bar);

    auto resetAllSpr = ButtonSprite::create(
        "Reset All", "bigFont.fnt", "GJ_button_05.png", .75f
    );
    resetAllSpr->setScale(.5f);
    auto resetAllBtn = CCMenuItemSpriteExtra::create(
        resetAllSpr, this, menu_selector(KeybindsLayer::onResetAll)
    );
    resetAllBtn->setPosition(m_size.width / 2 - 50.f, m_size.height / 2 - 20.f);
    m_buttonMenu->addChild(resetAllBtn);

    this->updateVisibility();

    return true;
}

void KeybindsLayer::updateVisibility() {
    auto oldHeight = m_scroll->m_contentLayer->getContentSize().height;
    for (auto& container : m_containers) {
        if (container != m_scroll->m_contentLayer && m_query.size()) {
            bool hasVisibleChildren = false;
            for (auto& child : CCArrayExt<CCNode>(container->getChildren())) {
                if (child->getID() != "fold-layer" && child->isVisible()) {
                    hasVisibleChildren = true;
                }
            }
            container->getChildByID("fold-layer")->setVisible(hasVisibleChildren);
            container->setVisible(hasVisibleChildren);
            if (!hasVisibleChildren) {
                container->setContentSize({ 0.f, 0.f });
                continue;
            }
        }
        if (
            container == m_scroll->m_contentLayer ||
            container->getParent() == m_scroll->m_contentLayer ||
            !static_cast<FoldableLayer*>(container->getParent())->isFolded()
        ) {
            if (auto child = container->getChildByID("fold-layer")) {
                child->setVisible(true);
            }
            container->setVisible(true);
        }
        auto size = container->getLayout()->getSizeHint(container);
        if (
            container == m_scroll->m_contentLayer &&
            size.height < m_scroll->getContentSize().height
        ) {
            size.height = m_scroll->getContentSize().height;
        }
        container->setContentSize(size);
        container->updateLayout();
    }
    m_scroll->scrollLayer(oldHeight - m_scroll->m_contentLayer->getContentSize().height);

    size_t visible = 0;
    for (auto& node : m_nodes) {
        if (nodeIsVisible(node)) {
            visible += 1;
        }
    }
    m_resultsLabel->setString(fmt::format("Showing {} bindings", visible).c_str());
}

void KeybindsLayer::updateAllBinds() {
    for (auto& node : m_nodes) {
        node->updateMenu();
    }
}

void KeybindsLayer::search(std::string const& query) {
    m_query = query;
    for (auto& node : m_nodes) {
        if (
            node->getParent() == m_scroll->m_contentLayer ||
            !static_cast<FoldableLayer*>(node->getParent())->isFolded()
        ) {
            node->setVisible(
                !query.size() ||
                fts::fuzzy_match_simple(query.c_str(), node->getMatchString().c_str())
            );
        }
    }
    this->updateVisibility();
}

void KeybindsLayer::textChanged(CCTextInputNode* input) {
    this->search(input->getString());
}

void KeybindsLayer::onResetAll(CCObject*) {
    createQuickPopup(
        "Reset All",
        "Are you sure you want to <cr>reset ALL bindings to default</c>?",
        "Cancel", "Reset",
        [=](auto*, bool btn2) {
            if (btn2) {
                for (auto& action : BindManager::get()->getAllBindables()) {
                    BindManager::get()->resetBindsToDefault(action.getID());
                }
                this->updateAllBinds();
            }
        }
    );
}

void KeybindsLayer::deselectSearchInput() {
    m_searchInput->getInput()->onClickTrackNode(false);
}

void KeybindsLayer::onDevice(DeviceEvent*) {
    this->updateAllBinds();
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
