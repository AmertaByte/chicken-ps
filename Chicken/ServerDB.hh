#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "enet/enet.h"
#include "json.hpp"
int totaluids, ServerIds;
ENetHost* server;

#define pInfo(peer) ((PlayerInfo*)(peer->data))
using namespace std;
int port, protocol;
string version, cdnpath, cdn;
using json = nlohmann::json;
vector<string> admin_perm;

#define cloth0 cloth_hair
#define cloth1 cloth_shirt
#define cloth2 cloth_pants
#define cloth3 cloth_feet
#define cloth4 cloth_face
#define cloth5 cloth_hand
#define cloth6 cloth_back
#define cloth7 cloth_mask
#define cloth8 cloth_necklace
#define cloth9 cloth_ances
#define STR16(x, y) (*(uint16_t*)(&(x)[(y)]))
#define STRINT(x, y) (*(int*)(&(x)[(y)]))
#define Property_nol 0
#define Property_NoSeed 1
#define Property_Dropless 2
#define Property_Beta 4
#define Property_Mod 8
#define Property_Untradable 16
#define Property_Wrenchable 32
#define Property_MultiFacing 64
#define Property_Permanent 128
#define Property_AutoPickup 256
#define Property_WorldLock 512
#define Property_NoSelf 1024
#define Property_RandomGrow 2048
#define Property_Public 4096

struct InventoryItem {
	__int16 itemID;
	__int16 itemCount;
};
struct PlayerInventory {
	vector<InventoryItem> items;
};
struct PlayerInfo {
	string tankIDName, tankIDPass = "";
	bool hidesb = false;
	int smstate, mstate = 0;
	long long int lastSB = 0;
	int wrenchtime= 0;
	int skinColor = 0x8295C3FF;
	string requestedName, rawName = "";
	string country = "";
	string displayName = "";
	int level = 1;
	int xp = 0;
	bool haveGrowId = false;
	int gems = 0;
	int dev = 0;
	int mod = 0;
	int netID, userID = 0;
	string world = "";
	string f = "";
	string protocol = "";
	string gameVersion = "";
	string cbits = "";
	string fz = "";
	string lmode = "";
	string playerage = "";
	string GDPR = "";
	string hash2 = "";
	string meta = "";
	string fhash = "";
	string rid = "";
	string platformid = "";
	string deviceversion = "";
	string realName = "";
	string hash = "";
	string playerip = "";
	long long int packetpersecond = 0;
	string metaip = "";
	string mac = "";
	string reconnect = "";
	bool joinClothesUpdated, isRotatedLeft = false;
	string wk = "";
	string zf = "";
	int peffect = 8421376;
	int x1, y1;
	bool isGhost = false;
	int currentInventorySize = 0;
	PlayerInventory inventory;
	bool isUpdating, hasLogon, ischeck = false;
	int totaldroppeditem;
	int x, y;
	int cloth_hair, cloth_shirt = 0; // 0
	int cloth_pants, cloth_feet = 0; // 2
	int cloth_face, cloth_hand = 0; // 4
	int cloth_back = 0; // 6
	int cloth_mask = 0; // 7
	int cloth_necklace = 0; // 8
	int cloth_ances = 0; // 9
	bool BisaTembus, canDoubleJump = false; // 1
	bool isInvisible = false; // 4
	bool noHands = false; // 8
	bool noEyes = false; // 16
	bool noBody = false; // 32
	bool devilHorns = false; // 64
	bool goldenHalo = false; // 128
	bool isFrozen = false; // 2048
	bool isCursed = false; // 4096
	bool muted = false; // 8192
	long long int lastpacketsentperms = 0;
	bool haveCigar = false; // 16384
	bool isShining = false; // 32768
	bool isZombie = false; // 65536
	bool isHitByLava = false; // 131072
	bool haveHauntedShadows = false; // 262144
	bool haveGeigerRadiation = false; // 524288
	bool haveReflector = false; // 1048576
	bool isEgged = false; // 2097152
	bool havePineappleFloag = false; // 4194304
	bool isIn = false;
	bool update_clothes = false;
	bool haveFlyingPineapple = false; // 8388608
	bool haveSuperSupporterName = false; // 16777216
	bool haveSupperPineapple = false; // 33554432
};

struct WorldItem {
	__int16 foreground = 0;
	__int16 background = 0;
	int breakLevel = 0;
	int displayblock;
	bool rotatedLeft = false;
	bool fire, glue, water = false;
	bool red, green, blue = false;
	long long int breakTime = 0;
	bool flipped, silenced, activated, active = false;
	vector<int> lock_x;
	vector<int> lock_y;
	int lock_owner_id = 0;
};

struct WorldInfo {
	int width = 100;
	int height = 60;
	string name = "TEST";
	int weather = 0;
	string owner = "";
	int dropped_item_count = 0;
	int ownerID = 0;
	WorldItem* items;
};

struct AWorld {
	WorldInfo* ptr;
	WorldInfo info;
	int id;
};

class WorldDB {
public:
	WorldInfo get(string name);
	int getworldStatus(string name);
	AWorld get2(string name);
	void flush(WorldInfo info);
	void flush2(AWorld info);
	void save(AWorld info);
	void SaveWorld();
	void saveRedundant();
	vector<WorldInfo> getRandomWorlds();
	WorldDB();
private:
	vector<WorldInfo> worlds;
};

WorldDB::WorldDB() {
	// Constructor
}

class PlayerDatabase {
public:
	static string getProperName(string name);
	static string FixColor(string text);
	static int Login(ENetPeer* peer, string name, string pass);
	static int Register(ENetPeer* peer, string name, string pass, string passverify, string email, string discord);
	static void Load(ENetPeer* peer);
};

enum Weather {
	SUNNY = 932,
	RAINY = 984,
	NIGHT = 934,
	WARP_SPEED = 1750,
	ARID = 946,
	SNOWY = 1364,
	SPOOKY = 1210
};