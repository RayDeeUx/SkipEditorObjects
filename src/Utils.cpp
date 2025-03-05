#include "Manager.hpp"
#include "Utils.hpp"
#include <random>
#include <regex>

static const std::regex numbersOnly(R"(^(\d+)[\n\r]*$)", std::regex::optimize | std::regex::icase);

namespace Utils {

    void initVector(bool showAlert) {
        std::vector<int> objIDs;
        auto pathCustom = (geode::Mod::get()->getConfigDir() / "custom.txt").string();
        std::ifstream file(pathCustom);
        std::string objID;
        std::smatch match;
        while (std::getline(file, objID)) {
            geode::log::debug("objID: {}", objID);
            if (!std::regex_match(objID, match, numbersOnly)) continue;
            const int element = geode::utils::numFromString<int>(objID).unwrapOr(-1);
            objIDs.push_back(element);
            geode::log::debug("added {} to IDs for skipping", element);
        }
        Manager::getSharedInstance()->theIDs = objIDs;
        if (showAlert) FLAlertLayer::create("Success!", "Your object IDs have been loaded.", "Close")->show();
    }

    int randomInt(const int& min, const int& max) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(min, max);
        return uni(rng);
    }

}
