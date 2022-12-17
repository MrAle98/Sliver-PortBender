#pragma once
#include "pch.h"
#include "PortBender.h"

class PortBenderWrapper
{
public:
	PortBenderWrapper(PortBender);
	~PortBenderWrapper();
	PortBenderWrapper(const PortBenderWrapper&);
	void start();
	void stop();
	std::tuple<UINT16, UINT16, OperatingMode, std::string> getData();

private:
	std::unique_ptr<std::thread> thread_ptr;
	PortBender portbender;
};