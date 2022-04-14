#define FMT_HEADER_ONLY
#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <string>
#include <chrono>
#include <fstream>
#include <utility>
#include <sstream>
#include <limits.h>
#include "json.hpp"
#include "ServerDB.hh"
#include "Visual.hh"
#include "server.hh"
#include "checking.hh"
#include "gamepackets.hh"
#include "checking.hh"
#include "Console.hh"
#include "fmt/format.h"
#include "fmt/core.h"
#include "fmt/printf.h"

char _getch() {
	return getchar();
}
#pragma comment(lib, "wininet.lib")

#pragma warning(disable: 4996)
using namespace std;

#pragma region Items.Dat

int HexToNumber(char x) {
	switch (x) {
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
		return 10;
	case 'B':
		return 11;
	case 'C':
		return 12;
	case 'D':
		return 13;
	case 'E':
		return 14;
	case 'F':
		return 15;
	default:
		break;
	}
}

#pragma endregion
void updateAllClothes(ENetPeer* p_) {
	ENetPeer* cp_;
	for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
		if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
		if (is_currently_here(p_, cp_)) {
			gamepacket_t p(0, pInfo(p_)->netID);
			p.Insert("OnSetClothing");
			p.Insert(pInfo(p_)->cloth_hair, pInfo(p_)->cloth_shirt, pInfo(p_)->cloth_pants);
			p.Insert(pInfo(p_)->cloth_feet, pInfo(p_)->cloth_face, pInfo(p_)->cloth_hand);
			p.Insert(pInfo(p_)->cloth_back, pInfo(p_)->cloth_mask, pInfo(p_)->cloth_necklace);
			p.Insert(pInfo(p_)->skinColor);
			p.Insert(pInfo(p_)->cloth_ances, 0.0f, 0.0f);
			p.CreatePacket(cp_);
			gamepacket_t p2(0, pInfo(p_)->netID);
			p2.Insert("OnSetClothing");
			p2.Insert(pInfo(p_)->cloth_hair, pInfo(p_)->cloth_shirt, pInfo(p_)->cloth_pants);
			p2.Insert(pInfo(p_)->cloth_feet, pInfo(p_)->cloth_face, pInfo(p_)->cloth_hand);
			p2.Insert(pInfo(p_)->cloth_back, pInfo(p_)->cloth_mask, pInfo(p_)->cloth_necklace);
			p2.Insert(pInfo(p_)->skinColor);
			p2.Insert(pInfo(p_)->cloth_ances, 0.0f, 0.0f);
			p2.CreatePacket(p_);
		}
	}
}

string PlayerDatabase::getProperName(string name) {
	string newS;
	for (char c : name) newS += (c >= 'A' && c <= 'Z') ? c - ('A' - 'a') : c;
	string ret;
	for (size_t i = 0; i < newS.length(); i++)
	{
		if (newS[i] == '`') i++; else ret += newS[i];
	}
	string ret2;
	for (char c : ret) if ((c >= 'a' && c <= 'z') or (c >= '0' && c <= '9')) ret2 += c;
	return ret2;
}

string PlayerDatabase::FixColor(string text) {
	string ret = "";
	int colorLevel = 0;
	for (size_t i = 0; i < text.length(); i++) {
		if (text[i] == '`') {
			ret += text[i];
			if (i + 1 < text.length())
				ret += text[i + 1];
			if (i + 1 < text.length() && text[i + 1] == '`') {
				colorLevel--;
			}
			else {
				colorLevel++;
			}
			i++;
		}
		else {
			ret += text[i];
		}
	}
	for (int i = 0; i < colorLevel; i++) {
		ret += "``";
	}
	for (int i = 0; i > colorLevel; i--) {
		ret += "`w";
	}
	return ret;
}

void Send_Server_Data(ENetPeer* p_, int number, char* data, int length) {
	ENetPacket* packet = enet_packet_create(0,
		length + 5,
		ENET_PACKET_FLAG_RELIABLE);
	memcpy(packet->data, &number, 4);
	if (data != NULL) memcpy(packet->data + 4, data, length);
	char zero = 0;
	memcpy(packet->data + 4 + length, &zero, 1);
	enet_peer_send(p_, 0, packet);
	enet_host_flush(server);
}

char* GetTextPointerFromPacket(ENetPacket* packet) {
	char nol = 0;
	memcpy(packet->data + packet->dataLength - 1, &nol, 1);
	return (char*)(packet->data + 4);
}

BYTE* GetStructPointerFromTankPacket(ENetPacket* packet)
{
	unsigned int packetLength = packet->dataLength;
	BYTE* result = NULL;
	if (packetLength >= 0x3C) {
		BYTE* packetData = packet->data;
		result = packetData + 4;
		if (*(BYTE*)(packetData + 16) & 8) {
			if (packetLength < *(int*)(packetData + 56) + 60) {
				SendLog("Packet too small for extended packet to be valid");
				SendLog("Sizeof float is 4.  TankUpdatePacket size: 56");
				result = 0;
			}
		}
		else {
			int nol = 0;
			memcpy(packetData + 56, &nol, 4);
		}
	}
	return result;
}

int GetMessageTypeFromPacket(ENetPacket* packet) {
	int result;
	if (packet->dataLength > 3u) result = *(packet->data);
	else {
		SendLog("Bad packet length, ignoring message");
		result = 0;
	}
	return result;
}

template <class T>
T f(T i) {
	static_assert(std::is_integral<T>::value, "Integral required.");
	return i;
}