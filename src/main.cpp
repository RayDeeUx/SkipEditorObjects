#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/utils/web.hpp>
#include "Settings.hpp"
#include "Manager.hpp"
#include "Utils.hpp"

#define IF_VECTOR_EMPTY_RETURN if (manager->isEmpty) return
#define VECTOR_DOES_CONTAIN_ID manager->theIDs.contains(id)
#define LOADER_LOADED Loader::get()->isModLoaded
#define MOD_IS_ENABLED Mod::get()->getSettingValue<bool>("enabled")
#define GET_MANAGER Manager* manager = Manager::getSharedInstance();
#define ADD_SETTINGS_BUTTON (void) Mod::get()->registerCustomSettingType

using namespace geode::prelude;

$on_mod(Loaded) {
	ADD_SETTINGS_BUTTON("configdir", &MyButtonSettingV3::parse);
	ADD_SETTINGS_BUTTON("resourcedir", &MyButtonSettingV3::parse);
	ADD_SETTINGS_BUTTON("refresh", &MyButtonSettingV3::parse);
	// code adapted with permission from dialouge handler original author thesillydoggo: https://discord.com/channels/911701438269386882/911702535373475870/1212633554345918514
	const auto path = geode::utils::string::pathToString(Mod::get()->getConfigDir() / "custom.txt");
	if (!std::filesystem::exists(path)) {
	std::string content = R"(hello there
this is the text file where you add object IDs
separate object IDs by new lines like you see in this file
i didn't include any object IDs in here by default
(that would only cause confusion for more users in the long run, let's be honest)
don't worry, the mod won't read any lines that arent exclusively numbers
+—————————————————————————————————————————— HOWEVER! ————————————————————————————————————————————+
|   If you have Object Groups by RaZoom installed, and then specify an ID as part of an object   |
|    group, *that object ID will NOT be skipped!* Please see the following tutorial for more:    |
|   https://github.com/RazoomGD/geode-object-groups/tree/main/Configuring%20Object%20Groups.md   |
+————————————————————————————————————————— END NOTICE ———————————————————————————————————————————+
have fun!
--raydeeux)";
		(void) utils::file::writeString(path, content);
	}
	const std::string& fileToParseSetting = Mod::get()->getSettingValue<std::string>("fileToParse");
	if (!Utils::checkIfFileToParseExists(fileToParseSetting)) Utils::initVector(false, true);
	else Utils::initVector(false, true, Mod::get()->getResourcesDir() / fmt::format("{}.txt", fileToParseSetting));
	listenForSettingChanges<std::string>("fileToParse", [](const std::string& fileToParse) {
		if (!Utils::checkIfFileToParseExists(fileToParse)) Utils::initVector(true, false);
		else Utils::initVector(true, false, Mod::get()->getResourcesDir() / fmt::format("{}.txt", fileToParse));
	});
}

class $modify(MyEditButtonBar, EditButtonBar) {

/*
	proof of consent for code adaptation and re-use:
	https://discord.com/channels/911701438269386882/911702535373475870/1229471210581000233
*/

	void loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
		if (!MOD_IS_ENABLED) return EditButtonBar::loadFromItems(p0, p1, p2, p3);
		if (LOADER_LOADED("iandyhd3.hideeditorobjects")) {
			Mod::get()->setSettingValue<bool>("enabled", false);
			return EditButtonBar::loadFromItems(p0, p1, p2, p3);
		}
		GET_MANAGER
		IF_VECTOR_EMPTY_RETURN EditButtonBar::loadFromItems(p0, p1, p2, p3);
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
				if (const int id = gameObject->m_objectID; VECTOR_DOES_CONTAIN_ID) continue;
				newArray->addObject(object);
			}
			if (customObjectFound) newArray->addObject(object); // skip custom objects
		}
		EditButtonBar::loadFromItems(newArray, p1, p2, p3);
	}
};

class $modify(MyEditorUI, EditorUI) {
	void toggleMode(CCObject* sender) {
		EditorUI::toggleMode(sender);
		if (!MOD_IS_ENABLED) return;
		if (sender->getTag() != 2) return;
		if (!LOADER_LOADED("razoom.object_groups")) return;
		GET_MANAGER IF_VECTOR_EMPTY_RETURN;
		if (Mod::get()->getSavedValue<bool>("shownObjectGroupsWarning") && Utils::randomInt() != 1) return;
		Mod::get()->setSavedValue<bool>("shownObjectGroupsWarning", true);
		geode::createQuickPopup(
			"Heads up!",
			"<cl>Object Groups by RaZoom</c> will not skip an object ID if you've included it inside an object group.\nClick \"Read Tutorial\" to learn how to configure Object Groups.",
			"Read Tutorial", "Close",
			[](FLAlertLayer* alert, bool secondButton) {
				if (secondButton) return;
				geode::utils::web::openLinkInBrowser("https://github.com/RazoomGD/geode-object-groups/tree/main/Configuring%20Object%20Groups.md");
			}
		);
	}
	CreateMenuItem* getCreateBtn(int id, int bg) {
		const auto result = EditorUI::getCreateBtn(id, bg);
		if (!MOD_IS_ENABLED || LOADER_LOADED("iandyhd3.hideeditorobjects")) return result;
		GET_MANAGER
		IF_VECTOR_EMPTY_RETURN result;
		if ((id == 914 || id == 1615) && VECTOR_DOES_CONTAIN_ID)
			return CreateMenuItem::create(CCSprite::create("blank.png"_spr), nullptr, nullptr, nullptr); // return nonexistent sprite to bait previous function hook to hide the button
		return result;
	}
};
