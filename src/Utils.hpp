#pragma once

namespace Utils {
    void initVector(bool showAlert = false, bool fromOnModLoaded = false, const std::filesystem::path& fileToParse = geode::Mod::get()->getConfigDir() / "custom.txt");
    int randomInt(const int& min = 1, const int& max = 987'654);
}