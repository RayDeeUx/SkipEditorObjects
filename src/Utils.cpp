#include "Manager.hpp"
#include "Utils.hpp"
#include <random>
#include <regex>

static const std::regex numbersOnly(R"(^(\d+).*$)", std::regex::optimize | std::regex::icase);

namespace Utils {
    void initVector(const bool showAlert, const bool fromOnModLoaded) {
        Manager* manager = Manager::getSharedInstance();
        manager->theIDs.clear();
        auto pathCustom = (geode::Mod::get()->getConfigDir() / "custom.txt").string();
        std::ifstream file(pathCustom);
        std::string objID;
        std::smatch match;
        while (std::getline(file, objID)) {
            if (!std::regex_match(objID, match, numbersOnly)) continue;
            if (const int element = geode::utils::numFromString<int>(static_cast<std::string>(match[0])).unwrapOr(-1); element != -1) manager->theIDs.push_back(element);
        }
        if (showAlert || fromOnModLoaded) manager->isEmpty = manager->theIDs.empty();
        if (showAlert) FLAlertLayer::create("Success!", fmt::format("Your object IDs have been loaded.\n\nDebug info:\n<cg>fromOnModLoaded</c>: {}\n<cg>manager->isEmpty</c>: {}", fromOnModLoaded, manager->isEmpty), "Close")->show();
    }

    int randomInt(const int& min, const int& max) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(min, max);
        return uni(rng);
    }
}
