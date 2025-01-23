#include "Manager.hpp"
#include "Utils.hpp"
#include <regex>

static const std::regex numbersOnly(R"(^(\d+)$)", std::regex::optimize | std::regex::icase);

namespace Utils {

    void initVector(bool showAlert) {
        std::vector<int> objIDs;
        auto pathCustom = (geode::Mod::get()->getConfigDir() / "custom.txt").string();
        std::ifstream file(pathCustom);
        std::string objID;
        std::smatch match;
        while (std::getline(file, objID))
            if (std::regex_match(objID, match, numbersOnly))
                objIDs.push_back(geode::utils::numFromString<int>(objID).unwrapOr(-1));
        Manager::getSharedInstance()->theIDs = objIDs;
        if (showAlert) FLAlertLayer::create("Success!", "Your object IDs have been loaded.", "Close")->show();
    }

}
