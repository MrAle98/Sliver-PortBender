#pragma once

#include <iostream>
#include <mutex>
#include <stdio.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <Windows.h>

#include "ConnectionManager.h"
#include "Packet.h"
#include "Utilities.h"
#include "WinDivert.h"

class PortBender {
public:
	PortBender(UINT16 FakeDstPort, UINT16 RedirectPort);
	PortBender(const PortBender&);
	PortBender(UINT16 FakeDstPort, UINT16 RedirectPort, std::string Password);
	void Start();
	void Stop();
	UINT16 getFakeDstPort();
	UINT16 getRedirectPort();
private:

	enum class OperatingMode {
		REDIRECTOR,
		BACKDOOR
	};
	std::mutex mut;
	bool stop;
	UINT16 FakeDstPort;
	OperatingMode Mode;
	std::string Password;
	UINT16 RedirectPort;

	PVOID ProcessPacket(Packet* packet, BOOL redirect);
};