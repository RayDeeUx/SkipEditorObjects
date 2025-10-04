#include "Manager.hpp"
#include "Utils.hpp"
#include <random>

namespace Utils {
    void initVector(const bool showAlert, const bool fromOnModLoaded, const std::filesystem::path& fileToParse) {
        Manager* manager = Manager::getSharedInstance();
        manager->theIDs.clear();
        std::ifstream file(fileToParse);
        std::string objID;
        while (std::getline(file, objID)) {
            objID = geode::utils::string::replace(geode::utils::string::replace(geode::utils::string::replace(objID, "\n", ""), " ", ""), "\r", "");
            if (!std::ranges::all_of(objID, [](auto c) { return std::isdigit(c); })) continue;
            if (const int element = geode::utils::numFromString<int>(objID).unwrapOr(-1); element != -1) manager->theIDs.insert(element);
        }
        if (showAlert || fromOnModLoaded) manager->isEmpty = manager->theIDs.empty();
        if (showAlert) FLAlertLayer::create("Success!", fmt::format("Your object IDs from {} have been loaded.\n\nDebug info:\n<cg>fromOnModLoaded</c>: {}\n<cg>manager->isEmpty</c>: {}", geode::utils::string::pathToString(fileToParse.filename()), fromOnModLoaded, manager->isEmpty), "Close")->show();
    }

    int randomInt(const int& min, const int& max) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(min, max);
        return uni(rng);
    }
}
