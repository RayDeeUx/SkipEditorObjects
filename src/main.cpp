#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <regex>

using namespace geode::prelude;

std::vector<int> theIDs;

const std::regex numbersOnly(R"(^(\d+)$)", std::regex::optimize | std::regex::icase);

$on_mod(Loaded) {
	// code adapted with permission from dialouge handler original author thesillydoggo: https://discord.com/channels/911701438269386882/911702535373475870/1212633554345918514
	auto path = (Mod::get()->getConfigDir() / "custom.txt").string();
	if (!std::filesystem::exists(path)) {
	std::string content = R"(hello there
this is the text file where you add object IDs
separate object IDs by new lines like you see in this file
i didn't include any object IDs in here by default
(that would only cause confusion for more users in the long run, let's be honest)
don't worry, the mod won't read any lines that arent exclusively numbers
have fun!
--raydeeux)";
		utils::file::writeString(path, content);
	}
}

void initVector() {
	std::vector<int> objIDs;
	auto pathCustom = (Mod::get()->getConfigDir() / "custom.txt").string();
	std::ifstream file(pathCustom);
	std::string objID;
	std::smatch match;
	while (std::getline(file, objID)) {
		if (std::regex_match(objID, match, numbersOnly))
		{
			objIDs.push_back(std::stoi(objID));
		}
	}
	theIDs = objIDs;
}

class $modify(MyEditButtonBar, EditButtonBar) {

/*
	proof of consent from iandyhd3 for code adaptation and re-use:
	https://discord.com/channels/911701438269386882/911702535373475870/1229471210581000233
*/

	void loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
		if (!Mod::get()->getSettingValue<bool>("enabled")) {
			EditButtonBar::loadFromItems(p0, p1, p2, p3);
		} else if (Loader::get()->isModLoaded("iandyhd3.hideeditorobjects")) {
			EditButtonBar::loadFromItems(p0, p1, p2, p3);
			FLAlertLayer::create(
				"SkipEditorObjects Disabled!",
				"You appear to have <cl>iAndyHD3's Hide Editor Objects</c> mod loaded. <cj>SkipEditorObjects</c> has deferred to your configs for <cl>Hide Editor Objects</c> instead.",
				"OK"
			)->show();
			Mod::get()->setSettingValue<bool>("enabled", false);
		} else {
			theIDs = {}; // clear the global vector and start anew
			initVector(); // clear the global vector and start anew
			auto newArray = CCArray::create();
			for (int i = 0; i < p0->count(); i++) {
				if (auto theObject = typeinfo_cast<CreateMenuItem*>(p0->objectAtIndex(i))) {
					if (auto buttonSprite = typeinfo_cast<ButtonSprite*>(theObject->getChildren()->objectAtIndex(0))) {
						auto buttonSpriteChildren = buttonSprite->getChildren();
						bool customObjectFound = true;
						for (int j = 0; j < buttonSprite->getChildrenCount(); j++) {
							if (auto gameObject = typeinfo_cast<GameObject*>(buttonSpriteChildren->objectAtIndex(j))) {
								customObjectFound = false;
								auto objIDInt = gameObject->m_objectID;
								if (std::find(theIDs.begin(), theIDs.end(), objIDInt) == theIDs.end()) {
									newArray->addObject(p0->objectAtIndex(i));
								}
							}
						}
						if (customObjectFound) {
							newArray->addObject(p0->objectAtIndex(i)); // skip custom objects
						}
					}
				} else {
					newArray->addObject(p0->objectAtIndex(i)); // skip editor controls
				}
			}
			EditButtonBar::loadFromItems(newArray, p1, p2, p3);
		}
	}
};

class $modify(MyEditorUI, EditorUI) {
	CreateMenuItem* getCreateBtn(int id, int bg) {
		auto result = EditorUI::getCreateBtn(id, bg);
		if (Mod::get()->getSettingValue<bool>("enabled") && !Loader::get()->isModLoaded("iandyhd3.hideeditorobjects")) {
			if (theIDs.size() < 1) initVector(); // only populate global vector if not done so already
			if ((id == 914 || id == 1615) && std::find(theIDs.begin(), theIDs.end(), id) != theIDs.end()) {
				return CreateMenuItem::create(CCSprite::create("blank.png"_spr), nullptr, nullptr, nullptr); // return nonexistent sprite to bait previous function hook to hide the button
			}
		}
		return result;
	}
};
