#pragma once
#include "pch.h"
#include "PortBenderWrapper.h"

class PortBenderManager
{
public:
	std::tuple<int, bool> add(PortBender);
	bool remove(int);
	//std::pair<UINT16, UINT16> getData(int);
	bool start(int);
	bool stop(int);
	std::vector<std::tuple<int, UINT16, UINT16, OperatingMode, std::string>> list();
private:
	std::map<int, std::shared_ptr<PortBenderWrapper>> map;
	int max;
};