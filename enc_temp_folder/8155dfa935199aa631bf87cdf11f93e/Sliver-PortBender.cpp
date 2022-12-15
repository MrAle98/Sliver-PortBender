// Sliver-PortBender.cpp : Definisce le funzioni esportate per la DLL.
//
#include "pch.h"
#include "Sliver-PortBender.h"
#include "Arguments.h"

#pragma warning(disable : 4996)

PortBenderWrapper::PortBenderWrapper(PortBender _portbender) : portbender(_portbender) {
    
}

PortBenderWrapper::~PortBenderWrapper() {
}

PortBenderWrapper::PortBenderWrapper(const PortBenderWrapper& source) : portbender(source.portbender) {

}

void PortBenderWrapper::start(){
   this->thread_ptr = std::make_unique<std::thread>(&PortBender::Start,&this->portbender);
}

void PortBenderWrapper::stop() {
    this->portbender.Stop();
    this->thread_ptr->join();
    this->thread_ptr.reset();
}

std::tuple<UINT16, UINT16, OperatingMode, std::string> PortBenderWrapper::getData(){
     return this->portbender.getData();
}

std::tuple<int, bool> PortBenderManager::add(PortBender portbender) {
    auto ret = this->manager.emplace(this->max, std::make_shared<PortBenderWrapper>(portbender));
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
    auto ret = this->manager.find(key);
    if (ret == this->manager.end()) {
        return false;
    }
    ret->second->start();
    return true;
}

bool PortBenderManager::remove(int key) {
    auto ret = this->manager.erase(key);
    if (ret == 0) {
        return false;
    }
    return true;
}

bool PortBenderManager::stop(int key) {
    auto ret = this->manager.find(key);
    if (ret == this->manager.end()) {
        return false;
    }
    ret->second->stop();
    return true;
}

//std::pair<UINT16,UINT16> PortBenderManager::getData(int key) {
//    auto ret = this->manager.find(key);
//    if (ret == this->manager.end()) {
//        return std::pair<UINT16,UINT16>(-1,-1);
//    }
//    return ret->second->getData();
//}

std::vector<std::tuple<int,UINT16, UINT16, OperatingMode, std::string>> PortBenderManager::list() {
    std::vector<std::tuple<int,UINT16, UINT16, OperatingMode, std::string>> vec;

    for (auto it = this->manager.begin(); it != this->manager.end(); ++it) {
        auto data = it->second->getData();
        auto tp = std::make_tuple(it->first, std::get<0>(data), std::get<1>(data), std::get<2>(data), std::get<3>(data));
        vec.push_back(tp);   
    }
    return vec;
}


std::unique_ptr<PortBenderManager> manager{ nullptr };

int func(const char* buff, int n) {
    return 0;
}

//Used for testing
int fakeEntryPoint() {
    char buff[100] = { '\0' };
    strcpy(buff, "ciao");
    entrypoint(buff, strlen(buff), func);
    strcpy(buff, "redirect 445 8445");
    entrypoint(buff, strlen(buff), func);
    strcpy(buff, "list");
    entrypoint(buff, strlen(buff), func);
    Sleep(1000 * 120);
    strcpy(buff, "remove 0");
    entrypoint(buff, strlen(buff), func);
    strcpy(buff, "list");
    entrypoint(buff, strlen(buff), func);
    return 0;
}

int entrypoint(char* argsBuffer, uint32_t bufferSize, goCallback callback)
{
    char buffer[1000] = { '\0' };

    if (manager == nullptr) {
        manager = std::make_unique<PortBenderManager>();
    }
    
    try {
        Arguments args = Arguments(argsBuffer);

        if (args.Action == "remove") {
            if (manager->stop(args.id) && manager->remove(args.id)) {
                sprintf(buffer, "successfully removed portBender with Id% d\n", args.id);
                callback(buffer, strlen(buffer));
            }
            else {
                sprintf(buffer, "unable to remove portBender with Id %d\n", args.id);
                callback(buffer, strlen(buffer));
            }
        }
        else if (args.Action == "backdoor") {
            /*std::cout << "Initializing PortBender in backdoor mode" << std::endl;
            PortBender backdoor = PortBender(args.FakeDstPort, args.RedirectPort, args.Password);
            backdoor.Start();*/
        }
        else if (args.Action == "redirect") {
            std::string msg("Initializing PortBender in redirector mode\n");
            callback(msg.c_str(), msg.length());
            auto tp = manager->add(PortBender(args.FakeDstPort, args.RedirectPort));
            if (std::get<1>(tp)) {
                manager->start(std::get<0>(tp));
            }
        }
        else if (args.Action == "list") {
            auto vec = manager->list();
            for (auto it = vec.begin(); it != vec.end(); ++it) {
                sprintf(buffer, "<id>: ORIGINAL DESTINATION PORT -> REDIRECTED PORT MODE PASSWORD\n");
                callback(buffer, strlen(buffer));
                printf("%s", buffer);
                sprintf(buffer, "%d: %d -> %d %d %s\n", 
                    std::get<0>(*it), std::get<1>(*it), 
                    std::get<2>(*it), std::get<3>(*it), 
                    std::get<4>(*it).c_str());
                callback(buffer, strlen(buffer));
                printf("%s", buffer);
            }
        }
    }
    catch (const std::invalid_argument&) {
        std::string msg("Redirect Usage : PortBender redirect FakeDstPort RedirectedPort\n");
        callback(msg.c_str(), msg.length());
        msg = "Backdoor Usage: PortBender backdoor FakeDstPort RedirectedPort password\n";
        callback(msg.c_str(), msg.length());
        msg = "Example:\n";
        callback(msg.c_str(), msg.length());
        msg = "\tPortBender redirect 445 8445\n";
        callback(msg.c_str(), msg.length());
        msg = "\tPortBender backdoor 443 3389 praetorian.antihacker\n";
        callback(msg.c_str(), msg.length());
    }
    catch (const std::exception& e) {
        std::string msg(e.what());
        callback(msg.c_str(), msg.length());
    }
    return 0;

}
