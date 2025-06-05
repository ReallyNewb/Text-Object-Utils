#include <Geode/Geode.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

class $modify(EditTextLayer, CustomizeObjectLayer) {
    struct Fields {
        TextInput* newTextInput;

        bool swapCopyPaste = mod->getSettingValue<bool>("swap-copy-paste-buttons");
        std::string lineShortcut = mod->getSettingValue<std::string>("new-line-shortcut");
    };

    bool init(GameObject* p0, CCArray* p1) {
        if (!CustomizeObjectLayer::init(p0, p1)) return false;
        auto defaultInput = m_textInput;
        if (!m_textInput) return true;

        auto& lineShortcut = m_fields->lineShortcut;
        if (lineShortcut.empty()) lineShortcut = "/n"; // fuck whatever user trys this


        auto textObject = static_cast<TextGameObject*>(p0);
        auto defaultInputBG = static_cast<CCScale9Sprite*>(m_mainLayer->getChildByID("text-input-bg"));
        defaultInputBG->setOpacity(0);
        m_kerningLabel->setPositionY(m_kerningLabel->getPositionY() - 20);
        m_kerningSlider->setPositionY(m_kerningSlider->getPositionY() - 20);
        defaultInput->setPositionY(500.0f); // fuck you im lazy
        this->getChildByIDRecursive("clear-text-button")->setPositionY(500.0f);
        
        m_textButton->activate();


        auto input = TextInput::create(200.0f, "");
        input->setPosition(defaultInputBG->getPositionX(), defaultInputBG->getPositionY() - 20);
        auto inputPos = input->getPosition();
        input->setCommonFilter(CommonFilter::Any);
        input->setString(textObject->m_text);
        input->setCallback([textObject, input, lineShortcut] (const std::string& inputString) {
            if (!textObject);
            else if (inputString.empty()) textObject->updateTextObject("a", false);
            else {
                auto str = inputString;
                int pos = 0;
                while ((pos = str.find(lineShortcut, pos)) != std::string::npos) {
                    str.replace(pos, lineShortcut.length(), "\n");
                    pos += 1;
                }
                textObject->updateTextObject(str, false);
                input->setString(str);
            }
        });
        input->setID("text-object-utils-input");
        m_mainLayer->addChild(input);
        m_textTabNodes->addObject(input); // so it hides
        m_fields->newTextInput = input;

        auto menu = CCMenu::create();
        menu->setPosition(0.0f, 0.0f);
        menu->setID("text-object-utils-menu");
        m_mainLayer->addChild(menu);
        m_textTabNodes->addObject(menu);

        auto copyButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_copyBtn_001.png"), this, menu_selector(EditTextLayer::onCopyText));
        copyButton->setPosition(inputPos.x + (m_fields->swapCopyPaste ? -120 : 120), inputPos.y);
        copyButton->setScale(0.5f);
        copyButton->m_baseScale = 0.5f;
        menu->addChild(copyButton);

        auto pasteButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_pasteBtn_001.png"), this, menu_selector(EditTextLayer::onPasteText));
        pasteButton->setPosition(inputPos.x + (m_fields->swapCopyPaste ? 120 : -120), inputPos.y);
        pasteButton->setScale(0.5f);
        pasteButton->m_baseScale = 0.5f;
        menu->addChild(pasteButton);

        auto clearButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png"), this, menu_selector(EditTextLayer::onClearText));
        clearButton->setPosition(inputPos.x + 150, inputPos.y);
        clearButton->setScale(0.5f);
        clearButton->m_baseScale = 0.5f;
        menu->addChild(clearButton);
        
        return true;
    }

    void onClose(CCObject* sender) {
        if (auto input = m_fields->newTextInput) input->getInputNode()->onClickTrackNode(false); // text inputs are unstable fucks that crash if deleted while selected
        CustomizeObjectLayer::onClose(sender);
    }

    void onCopyText(CCObject* sender) {
        auto input = m_fields->newTextInput;
        if (!input) return;
        clipboard::write(input->getString());
    }

    void onPasteText(CCObject* sender) {
        auto input = m_fields->newTextInput;
        if (!input) return;
        input->setString(clipboard::read(), true);
    }

    void onClearText(CCObject* sender) {
        auto input = m_fields->newTextInput;
        if (!input) return;
        input->setString("", true);
    }
};