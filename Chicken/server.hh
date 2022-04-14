#pragma once
#include "gamepackets.hh"
#include "Visual.hh"
#include <stdio.h>
#include "definitions.hh"
#include "Visual.hh"
#include "checking.hh"
#include "Console.hh"
#include <stdio.h>
#include <algorithm>
#include <string>
#include "lock.hh"

WorldDB worldDB;
WorldInfo* get_player_current_world(ENetPeer* peer) {
	try {
		return worldDB.get2(pInfo(peer)->world).ptr;
	}
	catch (...) {
		return NULL;
	}
}
WorldInfo WorldDB::get(string name) {
	return this->get2(name).info;
}
void SendWheel(ENetPeer* peer) {
	ENetPeer* cp_;
	int num = rand() % 37;
	for (cp_ = server->peers;
		cp_ < &server->peers[server->peerCount];
		++cp_)
	{
		if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
		if (is_currently_here(peer, cp_)) {
			gamepacket_t p(2500);
			p.Insert("OnTalkBubble");
			p.Insert(pInfo(peer)->netID);
			p.Insert("`w[" + pInfo(peer)->displayName + " `wspun the wheel and got `4" + to_string(num) + "`w!]");
			p.Insert(0);
			p.CreatePacket(cp_);
		}
	}
}
void WorldDB::flush(WorldInfo info) {
	try {
		ofstream o("Database/worlds/_" + info.name + "_.json");
		if (!o.is_open()) {
			cout << GetLastError() << endl;
		}
		json j;
		j["name"] = info.name;
		j["width"] = info.width;
		j["height"] = info.height;
		j["weather"] = info.weather;
		j["owner"] = info.owner;
		j["ownerID"] = info.ownerID;
		json tiles = json::array();
		int square = info.width * info.height;

		for (int i = 0; i < square; i++) {
			json tile;
			tile["fg"] = info.items[i].foreground;
			tile["bg"] = info.items[i].background;
			tiles.push_back(tile);
		}
		j["tiles"] = tiles;
		o << j << endl;
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}
}
WorldInfo generateWorld(string name, int width, int height)
{
	WorldInfo world;
	world.name = name;
	world.width = width;
	world.height = height;
	world.items = new WorldItem[world.width * world.height];
	for (int i = 0; i < world.width * world.height; i++) {
		if (i >= 3800 && i < 5400 && !(rand() % 50)) world.items[i].foreground = 10;
		else if (i >= 3700 && i < 5400) {
			if (i > 5000) {
				if (i % 7 == 0) world.items[i].foreground = 4;
				else world.items[i].foreground = 2;
			}
			else world.items[i].foreground = 2;
		}
		else if (i >= 5400) world.items[i].foreground = 8;
		if (i >= 3700) world.items[i].background = 14;
		if (i == 3650) world.items[i].foreground = 6;
		else if (i >= 3600 && i < 3700) world.items[i].foreground = 0; //fixed the grass in the world!
		if (i == 3750) world.items[i].foreground = 8;
	}
	return world;
}

void WorldDB::saveRedundant() {
	for (size_t i = 4; i < worlds.size(); i++) {
		bool canBeFree = true;
		ENetPeer* cp_;
		for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
			if (cp_->state != ENET_PEER_STATE_CONNECTED)
				continue;
			if (pInfo(cp_)->world == worlds.at(i).name) canBeFree = false;
		}
		if (canBeFree) {
			flush(worlds.at(i));
			delete worlds.at(i).items;
			worlds.erase(worlds.begin() + i);
			i--;
		}
	}
}

void WorldDB::SaveWorld() {
	for (size_t i = 0; i < worlds.size(); i++) {
		flush(worlds.at(i));
		delete[] worlds.at(i).items;
	}
	worlds.clear();
}

vector<WorldInfo> WorldDB::getRandomWorlds() {
	vector<WorldInfo> ret;
	for (size_t i = 0; i < ((worlds.size() < 10) ? worlds.size() : 10); i++) ret.push_back(worlds.at(i));
	// and lets get up to 6 random
	if (worlds.size() > 4) {
		for (int j = 0; j < 6; j++) {
			bool isPossible = true;
			WorldInfo world = worlds.at(rand() % (worlds.size() - 4));
			for (size_t i = 0; i < ret.size(); i++)
			{
				if (world.name == ret.at(i).name || world.name == "EXIT") isPossible = false;
			}
			if (isPossible) ret.push_back(world);
		}
	}
	return ret;
}

int TotalOfPeopleInWorld(string name) {
	int count = 0;
	ENetPeer* cp_;
	for (cp_ = server->peers;
		cp_ < &server->peers[server->peerCount];
		++cp_)
	{
		if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
		if (pInfo(cp_)->world == name) count++;
	}
	return count;
}

AWorld WorldDB::get2(string name) {
	if (worlds.size() > 200) {
#ifdef TOTAL_LOG
		cout << "[!] Saving redundant worlds!" << endl;
#endif
		saveRedundant();
#ifdef TOTAL_LOG
		cout << "[!] Redundant worlds are saved!" << endl;
#endif
	}
	AWorld ret;
	name = getStrUpper(name);
	if (name.length() < 1) throw 1; // too short name
	for (char c : name) {
		if ((c < 'A' || c>'Z') && (c < '0' || c>'9'))
			throw 2; // wrong name
	}
	if (name == "EXIT") {
		throw 3;
	}
	if (MulutAndaKotor(name)) throw 3;
	for (size_t i = 0; i < worlds.size(); i++) {
		if (worlds.at(i).name == name)
		{
			ret.id = i;
			ret.info = worlds.at(i);
			ret.ptr = &worlds.at(i);
			return ret;
		}

	}
	ifstream ifs("Database/worlds/_" + name + "_.json");
	if (ifs.is_open()) {
		json j;
		ifs >> j;
		WorldInfo info;
		info.name = j["name"].get<string>();
		info.width = j["width"].get<int>();
		info.height = j["height"].get<int>();
		info.weather = j["weather"].get<int>();
		info.owner = j["owner"].get<string>();
		info.ownerID = j["ownerID"].get<int>();
		json tiles = j["tiles"];
		int square = info.width * info.height;
		info.items = new WorldItem[square];
		for (int i = 0; i < square; i++) {
			info.items[i].foreground = tiles[i]["fg"].get<int>();
			info.items[i].background = tiles[i]["bg"].get<int>();
		}
		worlds.push_back(info);
		ret.id = worlds.size() - 1;
		ret.info = info;
		ret.ptr = &worlds.at(worlds.size() - 1);
		return ret;
	}
	else {
		WorldInfo info = generateWorld(name, 100, 60);

		worlds.push_back(info);
		ret.id = worlds.size() - 1;
		ret.info = info;
		ret.ptr = &worlds.at(worlds.size() - 1);
		return ret;
		throw 1;
	}
}
void sendInventory(ENetPeer* peer, PlayerInventory inventory) {
	try {
		int inventoryLen = inventory.items.size();
		int packetLen = 66 + (inventoryLen * 4) + 4;
		BYTE* data2 = new BYTE[packetLen];
		int MessageType = 0x4;
		int PacketType = 0x9;
		int NetID = -1;
		int CharState = 0x8;
		memset(data2, 0, packetLen);
		memcpy(data2, &MessageType, 4);
		memcpy(data2 + 4, &PacketType, 4);
		memcpy(data2 + 8, &NetID, 4);
		memcpy(data2 + 16, &CharState, 4);
		int endianInvVal = _byteswap_ulong(inventoryLen);
		memcpy(data2 + 66 - 4, &endianInvVal, 4);
		endianInvVal = _byteswap_ulong(pInfo(peer)->currentInventorySize);
		memcpy(data2 + 66 - 8, &endianInvVal, 4);
		int val = 0;
		for (int i = 0; i < inventoryLen; i++) {
			val = 0;
			val |= inventory.items.at(i).itemID;
			val |= inventory.items.at(i).itemCount << 16;
			val &= 0x00FFFFFF;
			val |= 0x00 << 24;
			memcpy(data2 + (i * 4) + 66, &val, 4);
		}
		ENetPacket* packet3 = enet_packet_create(data2, packetLen, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet3);
		delete[] data2;
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}
}
namespace Server {
	namespace Variant {
		void OnChangeSkin(ENetPeer* peer, int skinColor, int netID) {
			gamepacket_t p(0, netID);
			p.Insert("OnChangeSkin"), p.Insert(skinColor);
			p.CreatePacket(peer);
		}
		void OnZoomCamera(ENetPeer* peer, float value1, int value2) {
			gamepacket_t p;
			p.Insert("OnZoomCamera"), p.Insert(value1), p.Insert(value2);
			p.CreatePacket(peer);
		}
		void OnChangePureBeingMode(ENetPeer* peer, int mode) {
			gamepacket_t p;
			p.Insert("OnChangePureBeingMode"), p.Insert(mode);
			p.CreatePacket(peer);
		}
		void SmoothZoom(ENetPeer* peer) {
			gamepacket_t p;
			p.Insert("OnZoomCamera"), p.Insert(10000.0f), p.Insert(1000);
			p.CreatePacket(peer);
		}
		void OnRaceStart(ENetPeer* peer, int netID) {
			gamepacket_t p(0, netID);
			p.Insert("OnRaceStart");
			p.CreatePacket(peer);
		}
		void OnTransmutateLinkDataModified(ENetPeer* peer, string item1, string item2) {
			gamepacket_t p(0, pInfo(peer)->netID);
			p.Insert("OnTransmutateLinkDataModified"), p.Insert(item1 + ":" + item2);
			p.CreatePacket(peer);
		}
		void OnRaceEnd(ENetPeer* peer, int netID) {
			gamepacket_t p(0, netID);
			p.Insert("OnRaceEnd");
			p.CreatePacket(peer);
		}
		void OnBillboardChange(ENetPeer* peer, int netid, int itemid, bool BillBoard, int price, bool wlpeer, bool peerwl) {
			gamepacket_t p(0, netid);
			p.Insert("OnBillboardChange"), p.Insert(netid), p.Insert(itemid), p.Insert(BillBoard), p.Insert(price), p.Insert(wlpeer), p.Insert(peerwl);
			p.CreatePacket(peer);
		}
		void OnCountdownStart(ENetPeer* peer, int netID, int time, int score) {
			gamepacket_t p(0, netID);
			if (score == -1) {
				p.Insert("OnCountdownStart"), p.Insert(time);
			}
			else {
				p.Insert("OnCountdownStart"), p.Insert(time), p.Insert(time);
			}
			p.CreatePacket(peer);
		}
		void OnCountdownUpdate(ENetPeer* peer, int netID, int score) {
			gamepacket_t p(0, netID);
			p.Insert("OnCountdownUpdate"), p.Insert(score);
			p.CreatePacket(peer);
		}
		void OnCountryState(ENetPeer* peer, string country) {
			gamepacket_t p(0, pInfo(peer)->netID);
			p.Insert("OnCountryState"), p.Insert(country);
			p.CreatePacket(peer);
		}
		void OnRemove(ENetPeer* peer, string message) {
			gamepacket_t p;
			p.Insert("OnRemove"), p.Insert(message);
			p.CreatePacket(peer);
		}
		void OnPlayPositioned(ENetPeer* peer, string audiofile, int netID) {
			gamepacket_t p(0, netID);
			p.Insert("OnPlayPositioned"), p.Insert(audiofile),
				p.CreatePacket(peer);
		}
		void OnSetFreezeState(ENetPeer* peer, int state, int netID) {
			gamepacket_t p(0, netID);
			p.Insert("OnSetFreezeState"), p.Insert(state);
			p.CreatePacket(peer);
		}
		void OnSendToServer(ENetPeer* peer, int userID, int token, string ip, int port, string doorId, int lmode) {
			gamepacket_t p;
			p.Insert("OnSendToServer"), p.Insert(port), p.Insert(token), p.Insert(userID), p.Insert(ip + "|" + doorId), p.Insert(lmode);
			p.CreatePacket(peer);
		}
		void OnGuildDataChanged(ENetPeer* peer, int flag, int flag2, int flag3, int idk) {
			gamepacket_t p(0, pInfo(peer)->netID);
			p.Insert("OnGuildDataChange"), p.Insert(flag), p.Insert(flag2), p.Insert(flag3), p.Insert(idk);
			p.CreatePacket(peer);
		}
		void OnInvis(ENetPeer* peer, int state, int netID) {
			gamepacket_t p(0, netID);
			p.Insert("OnInvis"), p.Insert(state);
			p.CreatePacket(peer);
		}
		void OnSetPos(ENetPeer* peer, int netID, int x, int y, int ms) {
			gamepacket_t p(0, netID);
			p.Insert("OnSetPos"), p.Insert(x, y);
			p.CreatePacket(peer);
		}
		void SetHasGrowID(ENetPeer* peer, int status, string username, string password) {
			gamepacket_t p;
			p.Insert("SetHasGrowID"), p.Insert(status), p.Insert(username), p.Insert(password);
			p.CreatePacket(peer);
		}
		void OnStartAcceptLogon(ENetPeer* peer, int itemdathash, string path, string path2, string content, string etc) {
			gamepacket_t p;
			p.Insert("OnSuperMainStartAcceptLogonHrdxs47254722215a"), p.Insert(itemdathash), p.Insert(path), p.Insert(path2), p.Insert(content), p.Insert(etc);
			p.CreatePacket(peer);
		}
		void OnParticleEffect(ENetPeer* peer, int effect, float x, float y, int delay) {
			gamepacket_t p(delay);
			p.Insert("OnParticleEffect"), p.Insert(effect), p.Insert(x, y);
			p.CreatePacket(peer);
		}
		void OnStartTrade(ENetPeer* peer, string displayName, int netID) {
			gamepacket_t p;
			p.Insert("OnStartTrade"), p.Insert(displayName), p.Insert(netID);
			p.CreatePacket(peer);
		}
		void OnForceTradeEnd(ENetPeer* peer) {
			gamepacket_t p;
			p.Insert("OnForceTradeEnd");
			p.CreatePacket(peer);
		}
		void OnTradeStatus(ENetPeer* peer, int netID, string statustext, string items, string locked) {
			gamepacket_t p;
			p.Insert("OnTradeStatus"), p.Insert(netID), p.Insert(items), p.Insert(statustext), p.Insert(locked);
			p.CreatePacket(peer);
		}
		void OnRiftWings(ENetPeer* peer, int value1, int value2, int value3, int value4, int value5, int delay) {
			gamepacket_t p(0, pInfo(peer)->netID);
			p.Insert("OnRiftWings"), p.Insert(value1), p.Insert(value2), p.Insert(value3), p.Insert(value4), p.Insert(value5), p.Insert(delay);
			p.CreatePacket(peer);
		}
		void OnAddNotification(ENetPeer* peer, string text, string audiosound, string interfaceimage) {
			gamepacket_t p;
			p.Insert("OnAddNotification"), p.Insert(interfaceimage), p.Insert(text), p.Insert(audiosound), p.Insert(0);
			p.CreatePacket(peer);
		}
		void OnStorePurchaseResult(ENetPeer* peer, string message) {
			gamepacket_t p;
			p.Insert("OnStorePurchaseResult"), p.Insert(message);
			p.CreatePacket(peer);
		}
		void SendTalkSelf(ENetPeer* peer, string text) {
			gamepacket_t p;
			p.Insert("OnTalkBubble"), p.Insert(pInfo(peer)->netID), p.Insert(text), p.Insert(0), p.Insert(1);
			p.CreatePacket(peer);
		}
		void OnConsoleMessage(ENetPeer* peer, string text) {
			gamepacket_t p;
			p.Insert("OnConsoleMessage"), p.Insert(text);
			p.CreatePacket(peer);
		}
		void OnStoreRequest(ENetPeer* peer, string text) {
			gamepacket_t p;
			p.Insert("OnStoreRequest"), p.Insert(text);
			p.CreatePacket(peer);
		}
		void OnSpawn(ENetPeer* peer, string message) {
			gamepacket_t p;
			p.Insert("OnSpawn"), p.Insert(message);
			p.CreatePacket(peer);
		}
		void OnSetBux(ENetPeer* peer, int gems, int accountstate) {
			gamepacket_t p;
			p.Insert("OnSetBux"), p.Insert(gems), p.Insert(accountstate);
			p.CreatePacket(peer);
		}
		void OnSetCurrentWeather(ENetPeer* peer, int weather) {
			for (ENetPeer* cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(peer, cp_)) {
					gamepacket_t p;
					p.Insert("OnSetCurrentWeather"), p.Insert(weather);
					p.CreatePacket(cp_);
				}
			}
		}
		void OnNameChanged(ENetPeer* peer, int netID, string name) {
			for (ENetPeer* cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(peer, cp_)) {
					gamepacket_t p(0, netID);
					p.Insert("OnNameChanged"), p.Insert("`0`0" + name);
					p.CreatePacket(cp_);
				}
			}
		}
		void PlayAudio(ENetPeer* peer, string audioFile, int delayMS)
		{
			string text = "action|play_sfx\nfile|" + audioFile + "\ndelayMS|" + std::to_string(delayMS) + "\n";
			BYTE* data = new BYTE[5 + text.length()];
			BYTE nol = 0;
			int type = 3;
			memcpy(data, &type, 4);
			memcpy(data + 4, text.c_str(), text.length());
			memcpy(data + 4 + text.length(), &nol, 1);
			ENetPacket* packet = enet_packet_create(data,
				5 + text.length(),
				ENET_PACKET_FLAG_RELIABLE);

			enet_peer_send(peer, 0, packet);
			delete data;
		}
		void OnDialogRequest(ENetPeer* peer, string message) {
			gamepacket_t p;
			p.Insert("OnDialogRequest"), p.Insert(message);
			p.CreatePacket(peer);
		}
		void OnFailedToEnterWorld(ENetPeer* peer) {
			gamepacket_t p;
			p.Insert("OnFailedToEnterWorld");
			p.CreatePacket(peer);
		}

		void OnTextOverlay(ENetPeer* peer, string text) {
			gamepacket_t p;
			p.Insert("OnTextOverlay"), p.Insert(text);
			p.CreatePacket(peer);
		}

		void OnKilled(ENetPeer* peer) {
			gamepacket_t p(0, pInfo(peer)->netID);
			p.Insert("OnKilled");
			p.CreatePacket(peer);
		}
		void OnTalkBubble(ENetPeer* peer, int netID, string text, int chatColor, bool isOverlay) {
			if (isOverlay) {
				gamepacket_t p;
				p.Insert("OnTalkBubble"), p.Insert(pInfo(peer)->netID), p.Insert(text), p.Insert(chatColor), p.Insert(1);
				p.CreatePacket(peer);
			}
			else {
				gamepacket_t p;
				p.Insert("OnTalkBubble"), p.Insert(pInfo(peer)->netID), p.Insert(text), p.Insert(chatColor);
				p.CreatePacket(peer);
			}
		}
		void SetRespawnPos(ENetPeer* peer, int posX, int posY) {
			gamepacket_t p(0, pInfo(peer)->netID);
			p.Insert("SetRespawnPos"), p.Insert(posX + posY * 100);
			p.CreatePacket(peer);
		}
	}
	void onPeerConnect(ENetPeer* peer) {
		ENetPeer* cp_;

		for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
			if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
			if (peer != cp_) {
				if (is_currently_here(peer, cp_)) {
					string netIdS = to_string(pInfo(cp_)->netID);
					string userids = to_string(pInfo(cp_)->userID);
					Server::Variant::OnSpawn(peer, "spawn|avatar\nnetID|" + netIdS + "\nuserID|" + userids + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(pInfo(cp_)->x) + "|" + std::to_string(pInfo(cp_)->y) + "\nname|``" + pInfo(cp_)->displayName + "``\ncountry|" + pInfo(cp_)->country + "\ninvis|0\nmstate|" + to_string(pInfo(cp_)->mstate) + "\nsmstate|" + to_string(pInfo(cp_)->smstate) + "\n");
					string netIdS2 = to_string(pInfo(peer)->netID);
					Server::Variant::OnSpawn(cp_, "spawn|avatar\nnetID|" + netIdS2 + "\nuserID|" + userids + "\ncolrect|0|0|20|30\nposXY|" + std::to_string(pInfo(peer)->x) + "|" + std::to_string(pInfo(peer)->y) + "\nname|``" + pInfo(peer)->displayName + "``\ncountry|" + pInfo(peer)->country + "\ninvis|0\nmstate" + to_string(pInfo(peer)->mstate) + "\nsmstate|" + to_string(pInfo(peer)->smstate) + "\n");
				}
			}
		}
	}
	namespace Visual {
		namespace Packet {
			void send_normal(int a1, string a2, ENetPeer* enetPeer) {
				if (enetPeer) {
					ENetPacket* v3 = enet_packet_create(0, a2.length() + 5, 1);
					memcpy(v3->data, &a1, 4);
					memcpy((v3->data) + 4, a2.c_str(), a2.length());
					enet_peer_send(enetPeer, 0, v3);
				}
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
			void send_raw_v2(int a1, void* packetData, size_t packetDataSize, void* a4, ENetPeer* peer, int packetFlag) {
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
		}
		namespace State {
			int GetValue(PlayerInfo* info) {
				int val = 0;
				val |= info->BisaTembus << 0;
				val |= info->canDoubleJump << 1;
				val |= info->isInvisible << 2;
				val |= info->noHands << 3;
				val |= info->noEyes << 4;
				val |= info->noBody << 5;
				val |= info->devilHorns << 6;
				val |= info->goldenHalo << 7;
				val |= info->isFrozen << 11;
				val |= info->isCursed << 12;
				val |= info->muted << 13;
				val |= info->haveCigar << 14;
				val |= info->isShining << 15;
				val |= info->isZombie << 16;
				val |= info->isHitByLava << 17;
				val |= info->haveHauntedShadows << 18;
				val |= info->haveGeigerRadiation << 19;
				val |= info->haveReflector << 20;
				val |= info->isEgged << 21;
				val |= info->havePineappleFloag << 22;
				val |= info->haveFlyingPineapple << 23;
				val |= info->haveSuperSupporterName << 24;
				val |= info->haveSupperPineapple << 25;
				return val;
			}
			int GetCharacterState(PlayerInfo* pInfo) {
				int value = 0;
				if (not pInfo->haveGrowId) value = 50000;
				else value = 0;
				if (pInfo->cloth_hand == 6028) value = 1024;
				if (pInfo->cloth_hand == 6262) value = 8192;
				return value;
			}
			void update_player_state(ENetPeer* peer) {
				//return; // TODO
				ENetPeer* cp_;
				int state = Server::Visual::State::GetValue(pInfo(peer));
				int pro = Server::Visual::State::GetCharacterState(pInfo(peer));
				int statey = 0;
				if (pInfo(peer)->cloth_hand == 6028) statey = 1024;
				if (pInfo(peer)->cloth_hand == 6262) statey = 8192;
				if (not pInfo(peer)->haveGrowId) statey = 50000;
				for (cp_ = server->peers;
					cp_ < &server->peers[server->peerCount];
					++cp_)
				{
					if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
					if (is_currently_here(peer, cp_)) {
						PlayerMoving data;
						data.packetType = 0x14;
						data.characterState = statey;
						data.x = 1000;
						data.y = 100;
						data.punchX = 0;
						data.punchY = 0;
						data.XSpeed = 300;
						data.YSpeed = 600;
						data.netID = pInfo(peer)->netID;
						data.plantingTree = state;
						BYTE* raw = packPlayerMoving(&data);
						int var = pInfo(peer)->peffect; // punch effect
						memcpy(raw + 1, &var, 3);
						Server::Visual::Packet::send_raw(4, raw, 56, 0, cp_, ENET_PACKET_FLAG_RELIABLE);
					}
				}
				Server::Variant::OnNameChanged(peer, pInfo(peer)->netID, pInfo(peer)->displayName);
			}
		}
	}
	namespace World {
		void send_weather(ENetPeer* p_, WorldInfo* world_, int object_) {
			switch (object_) {
			case Weather::NIGHT:
				world_->weather = 2;
				break;
			case Weather::SNOWY:
				world_->weather = 11;
				break;
			case Weather::SUNNY:
				world_->weather = 4;
				break;
			case Weather::ARID:
				world_->weather = 3;
				break;
			case Weather::WARP_SPEED:
				world_->weather = 15;
				break;
			case Weather::SPOOKY:
				world_->weather = 8;
				break;
			case Weather::RAINY:
				world_->weather = 5;
				break;
			default:
				Server::Variant::OnConsoleMessage(p_, "That weather did not supported yet!");
				break;
			}
		}
		void send_world(ENetPeer* p_, WorldInfo* world_) {
			pInfo(p_)->update_clothes = false;
			string WorldName = world_->name;
			int world_x = world_->width;
			int world_y = world_->height;
			int kotak = world_x * world_y;
			__int16 name_length = WorldName.length();
			int alok = (8 * kotak);
			int total = 78 + name_length + kotak + 24 + alok;
			BYTE* data = new BYTE[total];
			int p = 4, q = 8, nol = 0;
			memset(data, 0, total);
			memcpy(data, &p, 1);
			memcpy(data + 4, &p, 1);
			memcpy(data + 16, &q, 1);
			memcpy(data + 66, &name_length, 1);
			memcpy(data + 68, WorldName.c_str(), name_length);
			memcpy(data + 68 + name_length, &world_x, 1);
			memcpy(data + 72 + name_length, &world_y, 1);
			memcpy(data + 76 + name_length, &kotak, 2);
			BYTE* block = data + 80 + name_length;
			for (int i = 0; i < kotak; i++) {
				memcpy(block, &nol, 2);
				memcpy(block + 2, &world_->items[i].background, 2);
				int type = 0x00000000;
				// type 1 = locked
				if (world_->items[i].water) type |= 0x04000000;
				if (world_->items[i].glue) type |= 0x08000000;
				if (world_->items[i].fire) type |= 0x10000000;
				if (world_->items[i].red) type |= 0x20000000;
				if (world_->items[i].green) type |= 0x40000000;
				if (world_->items[i].blue) type |= 0x80000000;
				memcpy(block + 4, &type, 4);
				block += 8;
			}
			ENetPacket* packetw = enet_packet_create(data, total, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(p_, 0, packetw);
			for (int i = 0; i < kotak; i++) {
				PlayerMoving data_;
				data_.packetType = 0x3;
				data_.characterState = 0x0; // animation
				data_.x = i % world_->width;
				data_.y = i / world_->height;
				data_.punchX = i % world_->width;
				data_.punchY = i / world_->width;
				data_.XSpeed = 0;
				data_.YSpeed = 0;
				data_.netID = -1;
				data_.plantingTree = world_->items[i].foreground;
				Visual::Packet::send_raw(4, packPlayerMoving(&data_), 56, 0, p_, ENET_PACKET_FLAG_RELIABLE);
			}
			pInfo(p_)->world = world_->name;
			if (pInfo(p_)->dev == 1) pInfo(p_)->displayName = "`6@" + pInfo(p_)->realName;
			else if (pInfo(p_)->mod == 1) pInfo(p_)->displayName = "`#@" + pInfo(p_)->realName;
			else pInfo(p_)->displayName = pInfo(p_)->rawName;
			Server::Variant::OnNameChanged(p_, pInfo(p_)->netID, pInfo(p_)->displayName);
			if (world_->owner != "") send_log(p_, "`5[`o" + WorldName + " World Locked by " + world_->owner + "`5]");
			delete[] data;
		}
		void sendNothingHappened(ENetPeer* peer, int x, int y) {
			PlayerMoving data_;
			data_.netID = pInfo(peer)->netID;
			data_.packetType = 0x8;
			data_.plantingTree = 0;
			data_.netID = -1;
			data_.x = x;
			data_.y = y;
			data_.punchX = x;
			data_.punchY = y;
			Server::Visual::Packet::send_raw(4, packPlayerMoving(&data_), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
		}
		void Update_Tile(int x, int y, int new_object, int causedBy, ENetPeer* p_, WorldInfo* world_) {
			if (new_object > (int)itemDefs.size()) return;
			PlayerMoving data_;
			//data_.packetType = 0x14;
			data_.packetType = 0x3;
			//data_.characterState = 0x924; // animation
			data_.characterState = 0x0; // animation
			data_.x = x;
			data_.y = y;
			data_.punchX = x;
			data_.punchY = y;
			data_.XSpeed = 0;
			data_.YSpeed = 0;
			data_.netID = causedBy;
			data_.plantingTree = new_object;
			int triggered_tile = world_->items[x + (y * world_->width)].foreground;
			if (getItemDef(new_object).BlockType == BlockTypes::CONSUMABLE) return;
			sendNothingHappened(p_, x, y);
			if (world_ == NULL) return;
			if (x < 0 or y < 0 or x > world_->width - 1 or y > world_->height - 1 or new_object > (int) itemDefs.size()) return; // needs - 1
			sendNothingHappened(p_, x, y);
			if (pInfo(p_)->dev == 0 or pInfo(p_)->mod == 0) {
				if (triggered_tile == 6 or triggered_tile == 8 or triggered_tile == 3760) return;
				if (new_object == 6 or new_object == 8 or new_object == 3760 or new_object == 6864) return;
			}
			if (world_->owner != "") {
				if (pInfo(p_)->rawName == world_->owner) {
					// WE ARE GOOD TO GO
					if (new_object == 32) {
						if (getItemDef(triggered_tile).BlockType == BlockTypes::LOCK and triggered_tile != 202 and triggered_tile != 204 and triggered_tile != 206 and triggered_tile != 4994) {
							Variant::OnDialogRequest(p_, "set_default_color|`o\n\nadd_label_with_icon|big|`wShould this world be publicly breakable?``|left|242|\n\nadd_spacer|small|\nadd_button_with_icon|world_Public|Public|noflags|2408||\nadd_button_with_icon|world_Private|Private|noflags|202||\nadd_spacer|small|\nadd_quick_exit|\nadd_button|chc0|Close|noflags|0|0|\nend_dialog|wl_edit|||"); // Added dialog name
						}
					}
				}
				else return;
				if (new_object == 242) return;
			}
			if (new_object == 32) return;
			if (new_object == 822) {
				world_->items[x + (y * world_->width)].water = !world_->items[x + (y * world_->width)].water;
				return;
			}
			if (new_object == 3062) {
				world_->items[x + (y * world_->width)].fire = !world_->items[x + (y * world_->width)].fire;
				return;
			}
			if (new_object == 1866) {
				world_->items[x + (y * world_->width)].glue = !world_->items[x + (y * world_->width)].glue;
				return;
			}
			ItemDefinition def;
			try {
				def = getItemDef(new_object);
				if (def.BlockType == BlockTypes::CLOTHING) return;
			}
			catch (...) {
				def.breakHits = 4;
				def.BlockType = BlockTypes::UNKNOWN;
#ifdef TOTAL_LOG
				cout << "Ugh, unsupported item " << new_object << endl;
#endif
			}
			if (new_object == 18) {
				if (world_->items[x + (y * world_->width)].background == 6864 and triggered_tile == 0) return;
				if (world_->items[x + (y * world_->width)].background == 0 and triggered_tile == 0) return;
				//data_.netID = -1;
				int tool = pInfo(p_)->cloth_hand;
				data_.packetType = 0x8;
				data_.plantingTree = (tool == 98 or tool == 1438 or tool == 4956) ? 8 : 6;
				int block = triggered_tile > 0 ? triggered_tile : world_->items[x + (y * world_->width)].background;
				//if (triggered_tile == 0) return;
				using namespace std::chrono;
				if ((duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world_->items[x + (y * world_->width)].breakTime >= 4000) {
					world_->items[x + (y * world_->width)].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
					world_->items[x + (y * world_->width)].breakLevel = 0; // TODO
					if (triggered_tile == 758) SendWheel(p_);
				}
				if (y < world_->height) {
					world_->items[x + (y * world_->width)].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
					world_->items[x + (y * world_->width)].breakLevel += (int)((tool == 98 or tool == 1438 or tool == 4956) ? 8 : 6); // TODO
				}
				if (y < world_->height && world_->items[x + (y * world_->width)].breakLevel >= getItemDef(block).breakHits * 6) { // TODO
					data_.packetType = 0x3;// 0xC; // 0xF // world_::HandlePacketnew_objectChangeRequest
					data_.plantingTree = 18;
					world_->items[x + (y * world_->width)].breakLevel = 0;
					if (triggered_tile != 0) {
						if (triggered_tile == 242) world_->owner = "";
						triggered_tile = 0;
					}
					else world_->items[x + (y * world_->width)].background = 6864;
				}
			}
			else {
				for (int i = 0; i < (int)pInfo(p_)->inventory.items.size(); i++)
				{
					if (pInfo(p_)->inventory.items.at(i).itemID == new_object) {
						if ((unsigned int)pInfo(p_)->inventory.items.at(i).itemCount > 1) pInfo(p_)->inventory.items.at(i).itemCount--;
						else pInfo(p_)->inventory.items.erase(pInfo(p_)->inventory.items.begin() + i);
					}
				}
				if (def.BlockType == BlockTypes::BACKGROUND) world_->items[x + (y * world_->width)].background = new_object;
				else {
					if (triggered_tile != 0) return;
					triggered_tile = new_object;
					if (new_object == 202 or new_object == 204 or new_object == 206 or new_object == 4994) {
						Lock::Apply(p_, x, y, new_object, pInfo(p_)->userID, 1, world_, false);
					}
					if (getItemDef(new_object).BlockType == BlockTypes::LOCK and new_object != 202 and new_object != 204 and new_object != 206 and new_object != 4994) {
						world_->owner = pInfo(p_)->rawName;
						ENetPeer* cp_;
						for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
							if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
							if (is_currently_here(p_, cp_)) send_log(cp_, "`5[`w" + world_->name + " `ohas been World Locked by " + pInfo(p_)->displayName + "`5]");
						}
					}
				}
				world_->items[x + (y * world_->width)].breakLevel = 0;
			}
			ENetPeer* cp_;
			for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(p_, cp_)) Visual::Packet::send_raw(4, packPlayerMoving(&data_), 56, 0, cp_, ENET_PACKET_FLAG_RELIABLE);
			}
		}
		void enter_world(ENetPeer* p_, string action, int x1, int y2) {
			try {
				SendLog("A join_world function called");
				WorldInfo info_ = worldDB.get(action);
				Server::World::send_world(p_, &info_);
				int x = 3040, y = 736;
				for (int j = 0; j < info_.width * info_.height; j++) {
					if (info_.items[j].foreground == 6) {
						x = (j % info_.width) * 32;
						y = (j / info_.width) * 32;
					}
				}
				if (x1 != 0 and y2 != 0) {
					x = x1;
					y = y2;
				}
				int id = 244, uid = pInfo(p_)->userID;
				int smstate = pInfo(p_)->smstate, mstate = pInfo(p_)->mstate;
				Server::Variant::OnSpawn(p_, "spawn|avatar\nnetID|" + to_string(ServerIds) + "\nuserID|" + to_string(uid) + "\ncolrect|0|0|20|30\nposXY|" + to_string(x) + "|" + to_string(y) + "\nname|``" + pInfo(p_)->displayName + "``\ncountry|" + pInfo(p_)->country + "|" + to_string(id) + "\ninvis|0\nmstate|" + to_string(mstate) + "\nsmstate|" + to_string(pInfo(p_)->dev == 1 or pInfo(p_)->mod == 1 ? smstate = 1 : smstate = 0) + "\ntype|local\n");
				pInfo(p_)->netID = ServerIds;
				onPeerConnect(p_);
				ServerIds++;
				sendInventory(p_, pInfo(p_)->inventory);
				int total = 0;
				WorldInfo* world_ = get_player_current_world(p_);
				ENetPeer* cp_;
				string name = "";
				for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
					if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
					total++;
				}
				{
				}
				int others = 0;
				for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
					if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
					if (is_currently_here(p_, cp_)) others++;
				}
				int other = others - 1;
				Server::Variant::OnConsoleMessage(p_, "`oWorld `0" + world_->name + " `oentered. There are `0" + std::to_string(other) + " `oother people here`7, `0" + to_string(total) + " `oonline.");
				for (cp_ = server->peers;
					cp_ < &server->peers[server->peerCount];
					++cp_)
				{
					if (cp_->state != ENET_PEER_STATE_CONNECTED)
						continue;
					if (is_currently_here(p_, cp_)) {
						{
							Server::Variant::OnConsoleMessage(cp_, "`5<`w" + pInfo(p_)->displayName + "`` `5entered, `w" + to_string(other) + "`` others here>``");
							Variant::PlayAudio(cp_, "audio/door_open.wav", 0);
						}
					}
				}
			}
			catch (int e) {
				switch (e) {
				case 1:
					pInfo(p_)->world = "EXIT";
					Server::Variant::OnConsoleMessage(p_, "You have exited the world.");
					break;
				case 2:
					pInfo(p_)->world = "EXIT";
					Variant::OnConsoleMessage(p_, "You have entered bad characters in the world name!");
					break;
				case 3:
					pInfo(p_)->world = "EXIT";
					Server::Variant::OnConsoleMessage(p_, "Exit from what? Click back if you're done playing.");
					break;
				default:
					pInfo(p_)->world = "EXIT";
					Server::Variant::OnConsoleMessage(p_, "I know this menu is magical and all, but it has its limitations! You can't visit this world!");
					break;
				}
			}
		}
		void handle(ENetPeer* p_, string action, int x = 0, int y = 0) {
			try {
				if (enable_register and not pInfo(p_)->haveGrowId) {
					Server::Variant::OnDialogRequest(p_, "text_scaling_string|Dirttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt|\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\nadd_spacer|small|\nadd_small_font_button|RulesAndPunishment|`4Rules & Punishment|noflags|0|0|\nadd_smalltext|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_smalltext|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|passwordverify|Password Verify||18|\nadd_smalltext|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_smalltext|We will never ask you for your password or email, never share it with anyone!|left|\nadd_smalltext|Your `wDiscord ID `owill be used for secondary verification if you lost access to your `wemail address`o! Please enter in such format: `wdiscordname#tag `oExample Ibord#1137. Your `wDiscord Tag `ocan be found in your `wDiscord account settings`o. if you don't have a discord account, you can fill in this column anything like `wYour GrowID#Random Number `o[Bobtickle#9999].|\nadd_text_input|discord|Discord||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
					Server::Variant::OnFailedToEnterWorld(p_);
					return;
				}
				SendLog("handle world function executed");
				transform(action.begin(), action.end(), action.begin(), ::toupper);
				if (Found_Symbol(action)) {
					send_log(p_, "Only letters and numbers allowed");
					Variant::OnFailedToEnterWorld(p_);
				}
				if (MulutAndaKotor(action)) {
					send_log(p_, "No badwords allowed");
					Variant::OnFailedToEnterWorld(p_);
				}
				if (action == "") {
					send_log(p_, "Where are we going?");
					Variant::OnFailedToEnterWorld(p_);
				}
				else {
					enter_world(p_, action, x, y);
				}
			}
			catch (exception& e) {
				cerr << e.what() << endl;
			}
		}
	}
	namespace Player {
		bool Has_Admin_Perm(ENetPeer* p_) {
			if (find(admin_perm.begin(), admin_perm.end(), pInfo(p_)->rawName) != admin_perm.end()) return true;
			else return false;
		}
		void SendTradeEffect(ENetPeer* peer, int id, int netIDsrc, int netIDdst, int timeMs) {
			PlayerMoving data;
			data.packetType = 0x13;
			data.punchX = id;
			data.punchY = id;
			BYTE* raw = packPlayerMoving(&data);
			int netIdSrc = netIDsrc;
			int netIdDst = netIDdst;
			int three = 3;
			int n1 = timeMs;
			memcpy(raw + 3, &three, 1);
			memcpy(raw + 4, &netIdDst, 4);
			memcpy(raw + 8, &netIdSrc, 4);
			memcpy(raw + 20, &n1, 4);
			for (ENetPeer* cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(peer, cp_)) {
					BYTE* raw2 = new BYTE[56];
					memcpy(raw2, raw, 56);
					Visual::Packet::send_raw(4, raw2, 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
				}
			}
			delete[] raw;
		}
		void Respawn(ENetPeer* peer, bool isDeadByTile) {
			int netID = pInfo(peer)->netID;
			if (not isDeadByTile) Server::Variant::OnKilled(peer);
			Server::Variant::OnSetFreezeState(peer, 0, netID);
			Server::Variant::OnSetFreezeState(peer, 1, netID);
			WorldInfo* world = get_player_current_world(peer);
			int x = 3040;
			int y = 736;
			if (not world) return;
			for (int i = 0; i < world->width * world->height; i++) {
				if (world->items[i].foreground == 6) {
					x = (i % world->width) * 32;
					y = (i / world->width) * 32;
				}
			}
			if (pInfo(peer)->ischeck) Server::Variant::OnSetPos(peer, pInfo(peer)->netID, x, y, 0);
			else Server::Variant::OnSetPos(peer, pInfo(peer)->netID, x, y, 0);
			Server::Variant::OnPlayPositioned(peer, "audio/teleport.wav", netID);
		}
		void Leave(ENetPeer* peer, PlayerInfo* player) {
			ENetPeer* cp_;
			gamepacket_t p;
			p.Insert("OnRemove");
			p.Insert("netID|" + std::to_string(player->netID) + "\n");
			gamepacket_t p2;
			p2.Insert("OnConsoleMessage");
			p2.Insert("`5<`w" + player->displayName + "`` left, `w" + std::to_string(TotalOfPeopleInWorld(player->world) - 1) + "`` others here>``");
			for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(peer, cp_)) {
					{
						p.CreatePacket(peer);
						{
							p.CreatePacket(cp_);
						}
					}
					{
						p2.CreatePacket(cp_);
					}
				}
			}
		}
		void Save(ENetPeer* p_) {
			if (pInfo(p_)->haveGrowId) {
				string username = PlayerDatabase::getProperName(pInfo(p_)->rawName);
				ifstream fg("Database/players/" + pInfo(p_)->rawName + "_.json");
				json j;
				fg >> j;
				fg.close();
				j["dev"] = pInfo(p_)->dev;
				j["mod"] = pInfo(p_)->mod;
				ofstream fs("Database/players/" + pInfo(p_)->rawName + "_.json");
				fs << j;
				fs.close();
			}
		}
		void update_clothes(ENetPeer* p_) {
			ENetPeer* cp_;
			gamepacket_t p(0, pInfo(p_)->netID);
			p.Insert("OnSetClothing");
			p.Insert(pInfo(p_)->cloth_hair, pInfo(p_)->cloth_shirt, pInfo(p_)->cloth_pants);
			p.Insert(pInfo(p_)->cloth_feet, pInfo(p_)->cloth_face, pInfo(p_)->cloth_hand);
			p.Insert(pInfo(p_)->cloth_back, pInfo(p_)->cloth_mask, pInfo(p_)->cloth_necklace);
			p.Insert(pInfo(p_)->skinColor);
			p.Insert(pInfo(p_)->cloth_ances, 0.0f, 0.0f);
			for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(p_, cp_)) {
					p.CreatePacket(cp_);
				}
			}
		}
	}
	namespace Inventory {
		void AddItem(int item_id, int item_quantity, ENetPeer* p_, bool& success) {
			size_t invsizee = pInfo(p_)->currentInventorySize;
			bool exist = false;
			if (pInfo(p_)->inventory.items.size() == invsizee) {
				Server::Variant::OnConsoleMessage(p_, "Your inventory is full! Please upgrade it on the store.");
				exist = true;
			}
			bool FullStock, isInInv = false;
			for (size_t i = 0; i < pInfo(p_)->inventory.items.size(); i++) {
				if (pInfo(p_)->inventory.items.at(i).itemID == item_id and pInfo(p_)->inventory.items.at(i).itemCount >= 200) {
					Server::Variant::OnConsoleMessage(p_, "You have reached the max count of the item!");
					FullStock = true;
				}
				if (pInfo(p_)->inventory.items.at(i).itemID == item_id and pInfo(p_)->inventory.items.at(i).itemCount < 200) isInInv = true;
			}

			if (FullStock or exist) success = false;
			else {
				success = true;
				ifstream iffff("Database/inventory/" + pInfo(p_)->rawName + "_.json");
				json jj;
				if (iffff.fail()) {
					iffff.close();
				}
				if (iffff.is_open()) {
				}
				iffff >> jj; //load
				ofstream oo("Database/inventory/" + pInfo(p_)->rawName + "_.json");
				if (not oo.is_open()) {
					cout << GetLastError() << endl;
				}
				if (not isInInv) {
					for (int i = 0; i < pInfo(p_)->currentInventorySize; i++) {
						int itemid = jj["items"][i]["itemid"];
						int quantity = jj["items"][i]["count"];
						if (itemid == 0 && quantity == 0) {
							jj["items"][i]["itemid"] = item_id;
							jj["items"][i]["count"] = item_quantity;
							break;
						}
					}
					oo << jj << endl;
					InventoryItem item;
					item.itemID = item_id;
					item.itemCount = item_quantity;
					pInfo(p_)->inventory.items.push_back(item);
					sendInventory(p_, pInfo(p_)->inventory);
				}
				else {
					for (int i = 0; i < pInfo(p_)->currentInventorySize; i++) {
						int itemid = jj["items"][i]["itemid"];
						int quantity = jj["items"][i]["count"];

						if (itemid == item_id) {
							jj["items"][i]["count"] = quantity + item_quantity;
							break;
						}
					}
					oo << jj << endl;
					for (size_t i = 0; i < pInfo(p_)->inventory.items.size(); i++) {
						if (pInfo(p_)->inventory.items.at(i).itemID == item_id) {
							pInfo(p_)->inventory.items.at(i).itemCount += item_quantity;
							sendInventory(p_, pInfo(p_)->inventory);
						}
					}
				}
			}
		}
		void RemoveItem(int item_id, int item_quantity, ENetPeer* p_) {
			ifstream iffff("Database/inventory/" + pInfo(p_)->rawName + "_.json");
			json jj;

			if (iffff.fail()) {
				iffff.close();
			}
			if (iffff.is_open()) {
			}
			iffff >> jj; //load
			ofstream oo("Database/inventory/" + pInfo(p_)->rawName + "_.json");
			if (not oo.is_open()) {
			}

			for (int i = 0; i < pInfo(p_)->currentInventorySize; i++) {
				int itemid = jj["items"][i]["itemid"];
				int quantity = jj["items"][i]["count"];
				if (itemid == item_id) {
					if (quantity - item_quantity == 0) {
						jj["items"][i]["itemid"] = 0;
						jj["items"][i]["count"] = 0;
					}
					else {
						jj["items"][i]["itemid"] = itemid;
						jj["items"][i]["count"] = quantity - item_quantity;
					}
					break;
				}
			}
			oo << jj << endl;
			for (size_t i = 0; i < pInfo(p_)->inventory.items.size(); i++) {
				if (pInfo(p_)->inventory.items.at(i).itemID == item_id) {
					if ((unsigned int)pInfo(p_)->inventory.items.at(i).itemCount > item_quantity and (unsigned int)pInfo(p_)->inventory.items.at(i).itemCount != item_quantity) pInfo(p_)->inventory.items.at(i).itemCount -= item_quantity;
					else pInfo(p_)->inventory.items.erase(pInfo(p_)->inventory.items.begin() + i);
					sendInventory(p_, pInfo(p_)->inventory);
					Server::Player::update_clothes(p_);
				}
			}
		}
		void FindItem(ENetPeer* p_, int item_id, int item_quantity, bool& found) {
			found = false;
			for (int i = 0; i < (int)pInfo(p_)->inventory.items.size(); i++) {
				if (pInfo(p_)->inventory.items.at(i).itemID == item_id and pInfo(p_)->inventory.items.at(i).itemCount >= item_quantity) {
					found = true;
					break;
				}
			}
		}
	}
	namespace InGame {
		void SendLobby(ENetPeer* p_) {
			string message = "default|";
			vector<WorldInfo> worlds = worldDB.getRandomWorlds();
			if (not pInfo(p_)->isIn) return;
			if (worlds.size() > 0) message += worlds[0].name;
			message += "\nadd_button|Showing: `wWorlds``|_catselect_|0.6|3529161471|\n";
			for (size_t i = 0; i < worlds.size(); i++) message += "add_floater|" + worlds[i].name + "|" + to_string(TotalOfPeopleInWorld(worlds[i].name)) + "|0.55|3529161471\n";
			gamepacket_t p;
			p.Insert("OnRequestWorldSelectMenu"), p.Insert(message);
			p.CreatePacket(p_);
		}
		void Get_All_Players_Name_in_world(ENetPeer* p_) {
			ENetPeer* cp_;
			for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(p_, cp_)) {
					if (pInfo(p_)->isInvisible) continue;
					Server::Variant::OnTalkBubble(p_, pInfo(cp_)->netID, pInfo(cp_)->displayName, 0, true);
				}
			}
		}
		void Send_Action(ENetPeer* peer, string text, int netID) {
			gamepacket_t p(0, netID);
			p.Insert("OnAction");
			p.Insert(text);
			p.CreatePacket(peer);
		}
		void Send_Player_Chat(ENetPeer* p_, int netID, string message) {
			if (message.length() == 0) return;
			for (char c : message)
				if (c < 0x18 or all_of(message.begin(), message.end(), isspace)) return;
			ENetPeer* cp_;
			for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				if (is_currently_here(p_, cp_)) {
					gamepacket_t p;
					gamepacket_t p1;
					if (pInfo(p_)->dev == 1) {
						Variant::OnConsoleMessage(cp_, "CP:_PL:0_OID:_CT:[W]_ `o<`w" + pInfo(p_)->displayName + "`o> `5" + message);
						Variant::OnTalkBubble(cp_, netID, "`5" + message, 0, true);
					}
					else if (pInfo(p_)->mod == 1) {
						Variant::OnConsoleMessage(cp_, "CP:_PL:0_OID:_CT:[W]_ `o<`w" + pInfo(p_)->displayName + "`o> `^" + message);
						Variant::OnTalkBubble(cp_, netID, "`^" + message, 0, true);
					}
					else {
						Variant::OnConsoleMessage(cp_, "CP:_PL:0_OID:_CT:[W]_ `o<`w" + pInfo(p_)->displayName + "`o> " + message);
						Variant::OnTalkBubble(cp_, netID, message, 0, true);
					}
				}
			}
		}
	}
}
