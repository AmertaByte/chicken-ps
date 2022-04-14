#pragma once
#include "ServerDB.hh"
#include "checking.hh"
#include <stdio.h>

struct BlockData {
	int packetType;
	int x;
	int y;
	int fg;
	int bg;
	int charState;
	int plength;
	int visual;
	int visual2;
};
BYTE* makeData(BlockData bdata) {
	BYTE* data = new BYTE[bdata.plength];
	memset(data, 0, bdata.plength);
	memcpy(data + 0, &bdata.packetType, 2);
	memcpy(data + 12, &bdata.charState, 2);
	memcpy(data + 44, &bdata.x, 4);
	memcpy(data + 48, &bdata.y, 4);
	memcpy(data + 56, &bdata.fg, 2);
	memcpy(data + 58, &bdata.bg, 2);
	memcpy(data + 60, &bdata.visual, 4);
	memcpy(data + 64, &bdata.visual2, 4);
	return data;
}

struct TileExtra {
	int packetType;
	int characterState;
	float objectSpeedX;
	int punchX;
	int punchY;
	int charStat;
	int blockid;
	int visual;
	int signs;
	int backgroundid;
	int displayblock;
	int time;
	int netID;
	int weatherspeed;
	int bpm;
	int unused1;
	int unused2;
	int unused3;
	//int bpm;
};

struct PlayerMoving {
	int packetType;
	int netID;
	float x;
	float y;
	int characterState;
	int plantingTree;
	float XSpeed;
	float YSpeed;
	int punchX;
	int punchY;
	int secondnetID;
};

struct BlockVisual {
	int packetType;
	int characterState;
	int punchX;
	int punchY;
	float x;
	float y;
	int plantingTree;
	float XSpeed;
	float YSpeed;
	int charStat;
	int visual;
	int displayblock, blockid;
	int signs;
	int backgroundid;
	int netID;
};

BYTE* packPlayerMoving(PlayerMoving* dataStruct)
{
	BYTE* data = new BYTE[64];
	for (int i = 0; i < 64; i++)
	{
		data[i] = 0;
	}
	memcpy(data, &dataStruct->packetType, 4);
	memcpy(data + 4, &dataStruct->netID, 4);
	memcpy(data + 12, &dataStruct->characterState, 4);
	memcpy(data + 20, &dataStruct->plantingTree, 4);
	memcpy(data + 24, &dataStruct->x, 4);
	memcpy(data + 28, &dataStruct->y, 4);
	memcpy(data + 32, &dataStruct->XSpeed, 4);
	memcpy(data + 36, &dataStruct->YSpeed, 4);
	memcpy(data + 44, &dataStruct->punchX, 4);
	memcpy(data + 48, &dataStruct->punchY, 4);
	return data;
}

string packPlayerMoving2(PlayerMoving* dataStruct)
{
	string data;
	data.resize(56);
	STRINT(data, 0) = dataStruct->packetType;
	STRINT(data, 4) = dataStruct->netID;
	STRINT(data, 12) = dataStruct->characterState;
	STRINT(data, 20) = dataStruct->plantingTree;
	STRINT(data, 24) = *(int*)&dataStruct->x;
	STRINT(data, 28) = *(int*)&dataStruct->y;
	STRINT(data, 32) = *(int*)&dataStruct->XSpeed;
	STRINT(data, 36) = *(int*)&dataStruct->YSpeed;
	STRINT(data, 44) = dataStruct->punchX;
	STRINT(data, 48) = dataStruct->punchY;
	return data;
}

void send_block_visual(ENetPeer* peer, int x, int y, bool forEveryone, WorldInfo* world_) {
	if (not world_) return;
	int i = y * world_->width + x;
	int block_state_flag = 0;
	if (world_->items[i].flipped) block_state_flag |= 0x00200000;
	if (world_->items[i].water) block_state_flag |= 0x04000000;
	if (world_->items[i].glue) block_state_flag |= 0x08000000;
	if (world_->items[i].fire) block_state_flag |= 0x10000000;
	if (world_->items[i].red) block_state_flag |= 0x20000000;
	if (world_->items[i].green) block_state_flag |= 0x40000000;
	if (world_->items[i].blue) block_state_flag |= 0x80000000;
	if (world_->items[i].active) block_state_flag |= 0x00400000;
	if (world_->items[i].silenced) block_state_flag |= 0x02400000;
}

BYTE* packBlockVisual(BlockVisual* dataStruct)
{
	BYTE* data = new BYTE[72];
	for (int i = 0; i < 72; i++)
	{
		data[i] = 0;
	}
	memcpy(data, &dataStruct->packetType, 4);
	memcpy(data + 8, &dataStruct->netID, 4);
	memcpy(data + 12, &dataStruct->characterState, 4);
	memcpy(data + 44, &dataStruct->punchX, 4);
	memcpy(data + 48, &dataStruct->punchY, 4);
	memcpy(data + 52, &dataStruct->charStat, 4);
	memcpy(data + 56, &dataStruct->blockid, 4);
	//memcpy(data + 58, &dataStruct->backgroundid, 4);
	memcpy(data + 60, &dataStruct->visual, 4);
	memcpy(data + 64, &dataStruct->displayblock, 4);


	return data;
}

PlayerMoving* unpackPlayerMoving(BYTE* data)
{
	PlayerMoving* dataStruct = new PlayerMoving;
	dataStruct->packetType = *(int*)(data);
	dataStruct->netID = *(int*)(data + 4);
	dataStruct->characterState = *(int*)(data + 12);
	dataStruct->plantingTree = *(int*)(data + 20);
	dataStruct->x = *(float*)(data + 24);
	dataStruct->y = *(float*)(data + 28);
	dataStruct->XSpeed = *(float*)(data + 32);
	dataStruct->YSpeed = *(float*)(data + 36);
	dataStruct->punchX = *(int*)(data + 44);
	dataStruct->punchY = *(int*)(data + 48);
	return dataStruct;
}
string packPlayerMoving3(PlayerMoving* dataStruct)
{
	string data;
	data.resize(56);
	STRINT(data, 0) = dataStruct->packetType;
	STRINT(data, 4) = dataStruct->netID;
	STRINT(data, 12) = dataStruct->characterState;
	STRINT(data, 20) = dataStruct->plantingTree;
	STRINT(data, 24) = *(int*)&dataStruct->x;
	STRINT(data, 28) = *(int*)&dataStruct->y;
	STRINT(data, 32) = *(int*)&dataStruct->XSpeed;
	STRINT(data, 36) = *(int*)&dataStruct->YSpeed;
	STRINT(data, 44) = dataStruct->punchX;
	STRINT(data, 48) = dataStruct->punchY;
	return data;
}
void send_raw(int a1, void* packetData, size_t packetDataSize, void* a4, ENetPeer* peer, int packetFlag) {
	ENetPacket* p;
	if (peer) {
		if (a1 == 4 && *((BYTE*)packetData + 12) & 8) {
			p = enet_packet_create(0, packetDataSize + *((DWORD*)packetData + 13) + 5, packetFlag);
			int four = 4;
			memcpy(p->data, &four, 4);
			memcpy((char*)p->data + 4, packetData, packetDataSize);
			memcpy((char*)p->data + packetDataSize + 4, a4, *((DWORD*)packetData + 13));
			enet_peer_send(peer, 0, p);
		}
		else {
			if (a1 == 192) {
				a1 = 4;
				p = enet_packet_create(0, packetDataSize + 5, packetFlag);
				memcpy(p->data, &a1, 4);
				memcpy((char*)p->data + 4, packetData, packetDataSize);
				enet_peer_send(peer, 0, p);
			}
			else {
				p = enet_packet_create(0, packetDataSize + 5, packetFlag);
				memcpy(p->data, &a1, 4);
				memcpy((char*)p->data + 4, packetData, packetDataSize);
				enet_peer_send(peer, 0, p);
			}
		}
	}
	delete packetData;
}
void Send_Player_Moving(ENetPeer* peer, PlayerMoving* data_) {
	ENetPeer* cp_;
	for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
		if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
		if (peer != cp_) {
			if (is_currently_here(peer, cp_)) {
				data_->netID = pInfo(peer)->netID;
				send_raw(4, packPlayerMoving(data_), 56, 0, cp_, ENET_PACKET_FLAG_RELIABLE);
			}
		}
	}
}

void tile_data(ENetPeer* p_, int x, int y, int visual, uint16_t fg_block, uint16_t bg_block, string tile_data) {
	PlayerMoving pmov_;
	pmov_.packetType = 5;
	pmov_.characterState = 0;
	pmov_.x = 0;
	pmov_.y = 0;
	pmov_.XSpeed = 0;
	pmov_.YSpeed = 0;
	pmov_.plantingTree = 0;
	pmov_.punchX = x;
	pmov_.punchY = y;
	pmov_.netID = 0;
	string packetstr;
	packetstr.resize(4);
	packetstr[0] = 4;
	packetstr += packPlayerMoving2(&pmov_);
	packetstr[16] = 8;
	packetstr.resize(packetstr.size() + 4);
	STRINT(packetstr, 52 + 4) = tile_data.size() + 4;
	STR16(packetstr, 56 + 4) = fg_block;
	STR16(packetstr, 58 + 4) = bg_block;
	packetstr += tile_data;
	ENetPacket* packet = enet_packet_create(&packetstr[0],
		packetstr.length(),
		ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(p_, 0, packet);
}

string lock_tile_data(int visual, uint32_t owner, uint32_t adminLength, uint32_t* admins, bool isPublic = false, uint8_t bpm = 0) {
	string data;
	data.resize(4 + 2 + 4 + 4 + adminLength * 4 + 8);
	if (bpm) data.resize(data.length() + 4);
	data[2] = 0x01;
	if (isPublic) data[2] |= 0x80;
	data[4] = 3;
	data[5] = visual; // or 0x02
	STRINT(data, 6) = owner;
	STRINT(data, 10) = adminLength;
	for (uint32_t i = 0; i < adminLength; i++) {
		STRINT(data, 14 + i * 4) = admins[i];
	}
	if (bpm) {
		STRINT(data, 10)++;
		STRINT(data, 14 + adminLength * 4) = -bpm;
	}
	return data;
}

uint8_t* lock_tile_data_v2(uint32_t owner, uint32_t adminLength, uint32_t* admins) {
	uint8_t* data = new uint8_t[4 + 2 + 4 + 4 + adminLength * 4 + 8];
	memset(data, 0, 4 + 2 + 4 + 4 + adminLength * 4 + 8);
	data[2] = 0x1;
	data[4] = 3;
	*(uint32_t*)(data + 6) = owner;

	*(uint32_t*)(data + 10) = adminLength;
	for (uint32_t i = 0; i < adminLength; i++) {
		*(uint32_t*)(data + 14 + i * 4) = admins[i];
	}
	return data;
}