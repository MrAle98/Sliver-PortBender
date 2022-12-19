#pragma once

#include <Windows.h>

class WinDivert {
public:
	WinDivert(char* filter);
	~WinDivert();
	Packet* Receive();
	Packet* TryReceive(int);
	void Send(PVOID data, DWORD length, WINDIVERT_ADDRESS addr);
private:
	HANDLE handle = NULL;
};