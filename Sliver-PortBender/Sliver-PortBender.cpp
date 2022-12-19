// Sliver-PortBender.cpp : Definisce le funzioni esportate per la DLL.
//
#include "pch.h"
#include "Sliver-PortBender.h"
#include "Arguments.h"

#pragma warning(disable : 4996)
//std::pair<UINT16,UINT16> PortBenderManager::getData(int key) {
//    auto ret = this->map.find(key);
//    if (ret == this->map.end()) {
//        return std::pair<UINT16,UINT16>(-1,-1);
//    }
//    return ret->second->getData();
//}

std::vector<std::tuple<int,UINT16, UINT16, OperatingMode, std::string>> PortBenderManager::list() {
    std::vector<std::tuple<int,UINT16, UINT16, OperatingMode, std::string>> vec;

    for (auto it = this->map.begin(); it != this->map.end(); ++it) {
        auto data = it->second->getData();
        auto tp = std::make_tuple(it->first, std::get<0>(data), std::get<1>(data), std::get<2>(data), std::get<3>(data));
        vec.push_back(tp);   
    }
    return vec;
}

int func(const char* buff, int n) {
    return 0;
}

std::unique_ptr<PortBenderManager> manager{ nullptr };

//Used for testing
int fakeEntryPoint() {
    char buff[100] = { '\0' };
    strcpy(buff, "ciao");
    entrypoint(buff, strlen(buff), func);
    strcpy(buff, "list");
    entrypoint(buff, strlen(buff), func);
    strcpy(buff, "redirect 135 9001");
    entrypoint(buff, strlen(buff), func);
    while (1) {
        Sleep(100 * 1000);
    }
    return 0;
}

int entrypoint(char* argsBuffer, uint32_t bufferSize, goCallback callback)
{
    char buffer[1000] = { '\0' };
    std::string msg{""};
    

    if (manager == nullptr) {
        msg.append("Initializing manager...\n");
        manager = std::make_unique<PortBenderManager>();
    }

    if (bufferSize < 1) {
        msg.append("Redirect Usage : portbender redirect FakeDstPort RedirectedPort\n");
        msg.append("Example:\n");
        msg.append("\tportbender redirect 445 8445\n");
        msg.append("\tportbender backdoor 443 3389 praetorian.antihacker\n");
        msg.append("List Usage : PortBender list\n");
        msg.append("Remove Usage: portbender remove\n");
        callback(msg.c_str(), msg.length());
        return 0;
    }
    try {
        Arguments args = Arguments(argsBuffer);
        if (args.invalid == true) {
            msg.append("Redirect Usage : portbender redirect FakeDstPort RedirectedPort\n");
            msg.append("Example:\n");
            msg.append("\tportbender redirect 445 8445\n");
            msg.append("\tportbender backdoor 443 3389 praetorian.antihacker\n");
            msg.append("List Usage : PortBender list\n");
            msg.append("Remove Usage: portbender remove <id> \n");
            callback(msg.c_str(), msg.length());
            return 0;
        }
        if (args.Action == "remove") {
            if (manager->stop(args.id) && manager->remove(args.id)) {
                sprintf(buffer, "successfully removed redirection with Id% d\n", args.id);
                callback(buffer, strlen(buffer));
            }
            else {
                sprintf(buffer, "unable to remove redirection with Id %d\n", args.id);
                callback(buffer, strlen(buffer));
            }
        }
        else if (args.Action == "backdoor") {
            /*std::cout << "Initializing PortBender in backdoor mode" << std::endl;
            PortBender backdoor = PortBender(args.FakeDstPort, args.RedirectPort, args.Password);
            backdoor.Start();*/
        }
        else if (args.Action == "redirect") {            
            auto tp = manager->add(PortBender(args.FakeDstPort, args.RedirectPort));
            sprintf(buffer,"Creating redirection with id %d...\n",std::get<0>(tp));
            msg.append(buffer);
            if (std::get<1>(tp) && manager->start(std::get<0>(tp))) {
                sprintf(buffer, "Redirection created successfully\n", std::get<1>(tp), std::get<0>(tp));
                msg.append(buffer);
            }
            callback(msg.c_str(), msg.size());
        }
        else if (args.Action == "list") {
            auto vec = manager->list();
            if (!vec.size()) {
                msg.append("Nothing to show\n");
                callback(msg.c_str(), msg.size());
            }
            else {
                sprintf(buffer, "\tid:\tORIGINAL_PORT -> REDIRECTED_PORT\tMODE(0=redirect,1=backdoor)\tPASSWORD\n");
                msg.append(buffer);
                for (auto it = vec.begin(); it != vec.end(); ++it) {
                    sprintf(buffer, "\t%d:\t%d -> %d\t%d\t%s\n",
                        std::get<0>(*it), std::get<1>(*it),
                        std::get<2>(*it), std::get<3>(*it),
                        std::get<4>(*it).c_str());
                    msg.append(buffer);
                }
                callback(msg.c_str(), msg.length());
            }
        }
    }
    catch (const std::exception&) {
        msg.append("Redirect Usage : portbender redirect FakeDstPort RedirectedPort\n");
        msg.append("Example:\n");
        msg.append("\tportbender redirect 445 8445\n");
        msg.append("\tportbender backdoor 443 3389 praetorian.antihacker\n");
        msg.append("List Usage : PortBender list\n");
        msg.append("Remove Usage: portbender remove <id> \n");
        callback(msg.c_str(), msg.length());
    }
    return 0;

}
