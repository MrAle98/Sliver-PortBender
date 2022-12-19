#pragma once

#include "pch.h"
#include <iostream>
#include <stdio.h>

#include "Packet.h"
#include "WinDivert.h"

#include "libwindivert.h"

WinDivert::WinDivert(char* filter) {
	this->handle = WinDivertOpen(filter, WINDIVERT_LAYER_NETWORK, 0, 0);
	if (this->handle == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_INVALID_PARAMETER)
		{
			throw std::exception("Error invalid filter syntax was used");
		}
		else if (GetLastError() == ERROR_ACCESS_DENIED) {
			throw std::exception("Failed to open the WinDivert device. Access denied");
		}
		throw std::exception("Failed to open the WinDivert device");
	
	}

	if (!WinDivertSetParam(this->handle, WINDIVERT_PARAM_QUEUE_LEN, 8192)) {
		throw std::exception("Failed to set packet queue length");
	}

	if (!WinDivertSetParam(this->handle, WINDIVERT_PARAM_QUEUE_TIME, 2048)) {
		throw std::exception("Failed to set packet queue time");
	}
}

WinDivert::~WinDivert() {
	CloseHandle(this->handle);
	this->handle = NULL;
}

Packet* WinDivert::TryReceive(int seconds) {
	Packet* packet = new Packet();

	OVERLAPPED o = {0};
	HANDLE hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

	if (hEvent == NULL) {
		return NULL;
	}
	o.hEvent = hEvent;

	if (!WinDivertRecvEx(this->handle, packet->packet, sizeof(packet->packet), 0,
		&packet->addr, &packet->packet_len, &o)) {
		//delete packet;
		//return nullptr;
	}

	
	if (!GetOverlappedResultEx(this->handle, &o, (LPDWORD)&packet->packet_len, seconds * 1000, FALSE)) {
		delete packet;
		return nullptr;
	}

	WinDivertHelperParsePacket(packet->packet, packet->packet_len,
		&packet->ip_header, &packet->ipv6_header,
		&packet->icmp_header, &packet->icmpv6_header,
		&packet->tcp_header, &packet->udp_header,
		&packet->payload, &packet->payload_len);

	if (packet->tcp_header == NULL) {
		std::cout << "Error this shouldn't happen" << std::endl;
		std::cout << std::flush;
		delete packet;
		return nullptr;
	}

	return packet;
}

Packet* WinDivert::Receive() {
	Packet* packet = new Packet();

	OVERLAPPED o;
	if (!WinDivertRecvEx(this->handle, packet->packet, sizeof(packet->packet), 0,
		&packet->addr, &packet->packet_len, &o)) {

	}	
		if (!WinDivertRecv(this->handle, packet->packet, sizeof(packet->packet),
		&packet->addr, &packet->packet_len)) {
		std::cout << "Failed to read packet (" << GetLastError() << ")" << std::endl;;
		std::cout << std::flush;
		return nullptr;
	}

	WinDivertHelperParsePacket(packet->packet, packet->packet_len,
		&packet->ip_header, &packet->ipv6_header,
		&packet->icmp_header, &packet->icmpv6_header,
		&packet->tcp_header, &packet->udp_header,
		&packet->payload, &packet->payload_len);

	if (packet->tcp_header == NULL) {
		std::cout << "Error this shouldn't happen" << std::endl;
		std::cout << std::flush;
		return nullptr;
	}

	return packet;
}

void WinDivert::Send(PVOID data, DWORD length, WINDIVERT_ADDRESS addr) {

	if (!WinDivertSend(this->handle, (PVOID)data, length, &addr, NULL))
	{
		std::cout << "Failed to reinject packet (" << GetLastError() << ")" << std::endl;
		std::cout << std::flush;
		return;
	}
}