#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <regex>
#include "Settings.hpp"

using namespace geode::prelude;

std::vector<int> theIDs;

static const std::regex numbersOnly(R"(^(\d+)$)", std::regex::optimize | std::regex::icase);

$on_mod(Loaded) {
	(void) Mod::get()->registerCustomSettingType("configdir", &MyButtonSettingV3::parse);
	(void) Mod::get()->registerCustomSettingType("resourcedir", &MyButtonSettingV3::parse);
	// code adapted with permission from dialouge handler original author thesillydoggo: https://discord.com/channels/911701438269386882/911702535373475870/1212633554345918514
	const auto path = (Mod::get()->getConfigDir() / "custom.txt").string();
	if (!std::filesystem::exists(path)) {
	std::string content = R"(hello there
this is the text file where you add object IDs
separate object IDs by new lines like you see in this file
i didn't include any object IDs in here by default
(that would only cause confusion for more users in the long run, let's be honest)
don't worry, the mod won't read any lines that arent exclusively numbers
have fun!
--raydeeux)";
		(void) utils::file::writeString(path, content);
	}
}

void initVector() {
	std::vector<int> objIDs;
	auto pathCustom = (Mod::get()->getConfigDir() / "custom.txt").string();
	std::ifstream file(pathCustom);
	std::string objID;
	std::smatch match;
	while (std::getline(file, objID))
		if (std::regex_match(objID, match, numbersOnly))
			objIDs.push_back(utils::numFromString<int>(objID).unwrapOr(-1));
	theIDs = objIDs;
}

class $modify(MyEditButtonBar, EditButtonBar) {

/*
	proof of consent for code adaptation and re-use:
	https://discord.com/channels/911701438269386882/911702535373475870/1229471210581000233
*/

	void loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
		if (!Mod::get()->getSettingValue<bool>("enabled")) return EditButtonBar::loadFromItems(p0, p1, p2, p3);
		if (Loader::get()->isModLoaded("iandyhd3.hideeditorobjects")) {
			EditButtonBar::loadFromItems(p0, p1, p2, p3);
			Mod::get()->setSettingValue<bool>("enabled", false);
			return FLAlertLayer::create(
				"SkipEditorObjects Disabled!",
				"You appear to have <cl>iAndyHD3's Hide Editor Objects</c> mod loaded. <cj>SkipEditorObjects</c> has deferred to your configs for <cl>Hide Editor Objects</c> instead.",
				"OK"
			)->show();
		}
		theIDs = {}; // clear the global vector and start anew
		initVector(); // clear the global vector and start anew
		const auto newArray = CCArray::create();
		for (CCObject* object : CCArrayExt<CCObject*>(p0)) {
			const auto theObject = typeinfo_cast<CreateMenuItem*>(object);
			if (!theObject) {
				newArray->addObject(object);
				continue;
			}
			const auto buttonSprite = typeinfo_cast<ButtonSprite*>(theObject->getChildren()->objectAtIndex(0));
			if (!buttonSprite) continue;
			bool customObjectFound = true;
			for (int j = 0; j < buttonSprite->getChildrenCount(); j++) {
				const auto gameObject = typeinfo_cast<GameObject*>(buttonSprite->getChildren()->objectAtIndex(j));
				if (!gameObject) continue;
				customObjectFound = false;
				if (std::ranges::find(theIDs.begin(), theIDs.end(), gameObject->m_objectID) != theIDs.end()) continue;
				newArray->addObject(object);
			}
			if (customObjectFound) newArray->addObject(object); // skip custom objects
		}
		EditButtonBar::loadFromItems(newArray, p1, p2, p3);
	}
};

class $modify(MyEditorUI, EditorUI) {
	CreateMenuItem* getCreateBtn(int id, int bg) {
		const auto result = EditorUI::getCreateBtn(id, bg);
		if (!Mod::get()->getSettingValue<bool>("enabled") || Loader::get()->isModLoaded("iandyhd3.hideeditorobjects")) return result;
		if (theIDs.empty()) initVector(); // only populate global vector if not done so already
		if ((id == 914 || id == 1615) && std::ranges::find(theIDs.begin(), theIDs.end(), id) != theIDs.end())
			return CreateMenuItem::create(CCSprite::create("blank.png"_spr), nullptr, nullptr, nullptr); // return nonexistent sprite to bait previous function hook to hide the button
		return result;
	}
};