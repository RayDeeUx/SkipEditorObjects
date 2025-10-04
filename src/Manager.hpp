#pragma once

class Manager {
protected:
    static Manager* instance;
public:
    std::unordered_set<int> theIDs;
    bool isEmpty = false;
    static Manager* getSharedInstance() {
        if (!instance) instance = new Manager();
        return instance;
    }
};