#include "pch.h"
#include "PortBenderManager.h"


std::tuple<int, bool> PortBenderManager::add(PortBender portbender) {
    auto ret = this->map.emplace(this->max, std::make_shared<PortBenderWrapper>(portbender));
    std::tuple<int, bool> tp;

    if (ret.second == true) {
        tp = std::make_tuple(this->max, true);
        this->max++;
    }
    else {
        tp = std::make_tuple(-1, false);

    }
    return tp;
}

bool PortBenderManager::start(int key) {
    auto ret = this->map.find(key);
    if (ret == this->map.end()) {
        return false;
    }
    ret->second->start();
    return true;
}

bool PortBenderManager::remove(int key) {
    auto ret = this->map.erase(key);
    if (ret == 0) {
        return false;
    }
    return true;
}

bool PortBenderManager::stop(int key) {
    auto ret = this->map.find(key);
    if (ret == this->map.end()) {
        return false;
    }
    ret->second->stop();
    return true;
}

