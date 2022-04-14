#pragma once
#include "main.hh"
#include "definitions.hh"
#include <vector>
#include <sys/stat.h>

bool enable_register = false;

#pragma warning(disable: 4996)
using namespace std;

BYTE* itemsDat = 0;
int itemsDatSize = 0;
BYTE* itemsDatNormal = 0;
int itemdathash;
int itemsDatSizeNormal = 0;
int itemdathashNormal;
int maxItems = 0;

vector<string> explode(const string& delimiter, const string& str)
{
	vector<string> arr;

	int strleng = str.length();
	int delleng = delimiter.length();
	if (delleng == 0)
		return arr;//no change

	int i = 0;
	int k = 0;
	while (i < strleng)
	{
		int j = 0;
		while (i + j < strleng && j < delleng && str[i + j] == delimiter[j])
			j++;
		if (j == delleng)//found delimiter
		{
			arr.push_back(str.substr(k, i - k));
			i += delleng;
			k = i;
		}
		else
		{
			i++;
		}
	}
	arr.push_back(str.substr(k, i - k));
	return arr;
}
unsigned char* getA(string fileName, int* pSizeOut, bool bAddBasePath, bool bAutoDecompress)
{
	unsigned char* pData = NULL;
	FILE* fp = fopen(fileName.c_str(), "rb");
	if (!fp)
	{
		cout << "File not found" << endl;
		if (!fp) return NULL;
	}

	fseek(fp, 0, SEEK_END);
	*pSizeOut = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	pData = (unsigned char*)new unsigned char[((*pSizeOut) + 1)];
	if (!pData)
	{
		printf("Out of memory opening %s?", fileName.c_str());
		return 0;
	}
	pData[*pSizeOut] = 0;
	fread(pData, *pSizeOut, 1, fp);
	fclose(fp);

	return pData;
}

void SendLog(string message) {
	cout << message << endl;
}
void ConfigReader() {
	ifstream Read("configurations.json");
	if (Read.is_open()) {
		json j;
		Read >> j;
		Read.close();
		try {
			port = j["port"].get<int>(); // Fix ambiguous error lol
			cdnpath = j["OnSuperMainAddress"].get<string>();
			cdn = j["cdn"].get<string>();
			protocol = j["protocol"].get<int>();
			version = j["version"].get<string>();
			admin_perm = j["accessed_users"].get<vector<string>>();
			enable_register = j["enable_register"].get<bool>();
			SendLog("Configurations are Loaded!");
		}
		catch (...) {
			SendLog("There are some errors while trying to parse configuration, Fixing it (Warning : every configurations will back into default so you may have to edit it again)");
			string config_contents = "{ \n\n\"port\": 17091, \n\"OnSuperMainAddress\": \"ubistatic-a.akamaihd.net\", \n\"cdn\": \"0098/69605/cache/\", \n\"protocol\": 0, \n\"version\": \"3.84\", \n\"accesed_users\": [], \n\"enable_register\": false\n\n}";
			ofstream pro("configurations.json");
			pro << config_contents;
			pro.close();
			SendLog("Configurations have been fixed! Reloading it.");
			ifstream Read("configurations.json");
			json j;
			Read >> j;
			Read.close();
			port = j["port"].get<int>(); // Fix ambiguous error lol
			cdnpath = j["OnSuperMainAddress"].get<string>();
			cdn = j["cdn"].get<string>();
			protocol = j["protocol"].get<int>();
			version = j["version"].get<string>();
			admin_perm = j["accessed_users"].get<vector<string>>();
			enable_register = j["enable_register"].get<bool>();
			SendLog("Configurations are Loaded!");
		}
	}
	else {
		SendLog("Seems like the configurations are not found, creating it...");
		string config_contents = "{ \n\n\"port\": 17091, \n\"OnSuperMainAddress\": \"ubistatic-a.akamaihd.net\", \n\"cdn\": \"0098/69605/cache/\", \n\"protocol\": 0, \n\"version\": \"3.84\", \n\"accesed_users\": [], \n\"enable_register\": false\n\n}";
		ofstream pro("configurations.json");
		pro << config_contents;
		pro.close();
		SendLog("Configurations have been created! Reloading it.");
		ifstream Read("configurations.json");
		json j;
		Read >> j;
		Read.close();
		port = j["port"].get<int>(); // Fix ambiguous error lol
		cdnpath = j["OnSuperMainAddress"].get<string>();
		cdn = j["cdn"].get<string>();
		protocol = j["protocol"].get<int>();
		version = j["version"].get<string>();
		admin_perm = j["accessed_users"].get<vector<string>>();
		enable_register = j["enable_register"].get<bool>();
		SendLog("Configurations are Loaded!");
	}
}
void GetItemDescriptions() {
	int current = -1;
	ifstream infile("Descriptions.txt");
	for (string line; getline(infile, line);)
	{
		if (line.length() > 3 && line[0] != '/' && line[1] != '/')
		{
			vector<string> ex = explode("|", line);
			ItemDefinition def;
			if (atoi(ex[0].c_str()) + 1 < (int)itemDefs.size())
			{
				itemDefs.at(atoi(ex[0].c_str())).description = ex[1];
				if (not (atoi(ex[0].c_str()) % 2))
					itemDefs.at(atoi(ex[0].c_str()) + 1).description = "This is a tree.";
			}
		}
	}
}
void SendEffectMessage() {
	int current = -1;
	std::ifstream infile("effect.txt");
	for (string line; getline(infile, line);)
	{
		if (line.length() > 5 && line[0] != '/' && line[1] != '/')
		{
			vector<string> ex = explode("|", line);
			ItemDefinition def;
			itemDefs.at(atoi(ex[0].c_str())).effect = ex[3] + " `$(`o" + ex[1] + " `omod removed)";
			itemDefs.at(atoi(ex[0].c_str())).effects = ex[2] + " `$(`o" + ex[1] + " `omod added)";
		}
	}
}
uint32_t HashString(unsigned char* str, int len)
{
	if (!str) return 0;

	unsigned char* n = (unsigned char*)str;
	uint32_t acc = 0x55555555;

	if (len == 0) {
		while (*n)
			acc = (acc >> 27) + (acc << 5) + *n++;
	}
	else {
		for (int i = 0; i < len; i++) {
			acc = (acc >> 27) + (acc << 5) + *n++;
		}
	}
	return acc;
}
ifstream::pos_type filesize(string filename)
{
	ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

void BuildDatabase() {
	string secret = "PBG892FXX982ABC*";
	ifstream file("items.dat", std::ios::binary | std::ios::ate);
	int size = file.tellg();
	itemsDatSize = size;
	char* data = new char[size];
	file.seekg(0, std::ios::beg);
	if (file.read((char*)(data), size)) {
		itemsDat = new BYTE[60 + size];
		int MessageType = 0x4;
		int PacketType = 0x10;
		int NetID = -1;
		int CharState = 0x8;
		memset(itemsDat, 0, 60);
		memcpy(itemsDat, &MessageType, 4);
		memcpy(itemsDat + 4, &PacketType, 4);
		memcpy(itemsDat + 8, &NetID, 4);
		memcpy(itemsDat + 16, &CharState, 4);
		memcpy(itemsDat + 56, &size, 4);
		file.seekg(0, std::ios::beg);
		if (file.read((char*)(itemsDat + 60), size)) {
			uint8_t* pData;
			int size = 0;
			const char filename[] = "items.dat";
			size = filesize(filename);
			pData = getA((string)filename, &size, false, false);
			itemdathash = HashString((unsigned char*)pData, size);
			SendLog("Updated Items! Hash : " + to_string(itemdathash));
			file.close();
		}
	}
	else {
		SendLog("Failed to update item!");
		exit(0);
	}
	int itemCount;
	int memPos = 0;
	int16_t itemsdatVersion = 0;
	memcpy(&itemsdatVersion, data + memPos, 2);
	memPos += 2;
	memcpy(&itemCount, data + memPos, 4);
	memPos += 4;
	for (int i = 0; i < itemCount; i++) {
		ItemDefinition tile;
		{
			memcpy(&tile.id, data + memPos, 4);
			memPos += 4;
		}
		{
			tile.editableType = data[memPos];
			memPos += 1;
		}
		{
			tile.itemCategory = data[memPos];
			memPos += 1;
		}
		{
			tile.actionType = data[memPos];
			memPos += 1;
		}
		{
			tile.hitSoundType = data[memPos];
			memPos += 1;
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.name += data[memPos] ^ (secret[(j + tile.id) % secret.length()]);
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.texture += data[memPos];
				memPos++;
			}
		}
		memcpy(&tile.textureHash, data + memPos, 4);
		memPos += 4;
		tile.itemKind = memPos[data];
		memPos += 1;
		memcpy(&tile.val1, data + memPos, 4);
		memPos += 4;
		tile.textureX = data[memPos];
		memPos += 1;
		tile.textureY = data[memPos];
		memPos += 1;
		tile.spreadType = data[memPos];
		memPos += 1;
		tile.isStripeyWallpaper = data[memPos];
		memPos += 1;
		tile.collisionType = data[memPos];
		memPos += 1;
		tile.breakHits = data[memPos] / 6;
		memPos += 1;
		memcpy(&tile.dropChance, data + memPos, 4);
		memPos += 4;
		tile.ClothingType = data[memPos];
		memPos += 1;
		memcpy(&tile.rarity, data + memPos, 2);
		memPos += 2;
		tile.maxAmount = data[memPos];
		memPos += 1;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.extraFile += data[memPos];
				memPos++;
			}
		}
		memcpy(&tile.extraFileHash, data + memPos, 4);
		memPos += 4;
		memcpy(&tile.audioVolume, data + memPos, 4);
		memPos += 4;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.petName += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.petPrefix += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.petSuffix += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.petAbility += data[memPos];
				memPos++;
			}
		}
		{
			tile.seedBase = data[memPos];
			memPos += 1;
		}
		{
			tile.seedOverlay = data[memPos];
			memPos += 1;
		}
		{
			tile.treeBase = data[memPos];
			memPos += 1;
		}
		{
			tile.treeLeaves = data[memPos];
			memPos += 1;
		}
		{
			memcpy(&tile.seedColor, data + memPos, 4);
			memPos += 4;
		}
		{
			memcpy(&tile.seedOverlayColor, data + memPos, 4);
			memPos += 4;
		}
		memPos += 4; // deleted ingredients
		{
			memcpy(&tile.growTime, data + memPos, 4);
			memPos += 4;
		}
		memcpy(&tile.val2, data + memPos, 2);
		memPos += 2;
		memcpy(&tile.isRayman, data + memPos, 2);
		memPos += 2;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.extraOptions += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.texture2 += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				tile.extraOptions2 += data[memPos];
				memPos++;
			}
		}
		memPos += 80;
		if (itemsdatVersion >= 11) {
			{
				int16_t strLen = *(int16_t*)&data[memPos];
				memPos += 2;
				for (int j = 0; j < strLen; j++) {
					tile.punchOptions += data[memPos];
					memPos++;
				}
			}
		}
		if (itemsdatVersion >= 12) memPos += 13;
		if (itemsdatVersion >= 13) memPos += 4;
		if (itemsdatVersion >= 14) memPos += 4;
		if (i != tile.id)
			cout << "Item are unordered!" << i << "/" << tile.id << endl;

		switch (tile.actionType) {
		case 0:
			tile.BlockType = BlockTypes::FIST;
			break;
		case 1:
			// wrench tool
			break;
		case 2:
			tile.BlockType = BlockTypes::DOOR;
			break;
		case 3:
			tile.BlockType = BlockTypes::LOCK;
			break;
		case 4:
			tile.BlockType = BlockTypes::GEM;
			break;
		case 8:
			tile.BlockType = BlockTypes::CONSUMABLE;
			break;
		case 9:
			tile.BlockType = BlockTypes::GATEWAY;
			break;
		case 10:
			tile.BlockType = BlockTypes::SIGN;
			break;
		case 13:
			tile.BlockType = BlockTypes::MAIN_DOOR;
			break;
		case 15:
			tile.BlockType = BlockTypes::BEDROCK;
			break;
		case 17:
			tile.BlockType = BlockTypes::FOREGROUND;
			break;
		case 18:
			tile.BlockType = BlockTypes::BACKGROUND;
			break;
		case 19:
			tile.BlockType = BlockTypes::SEED;
			break;
		case 20:
			tile.BlockType = BlockTypes::CLOTHING;
			switch (tile.clothing) {
			case 0: tile.clothing = ClothTypes::HAIR;
				break;
			case 1: tile.clothing = ClothTypes::SHIRT;
				break;
			case 2: tile.clothing = ClothTypes::PANTS;
				break;
			case 3: tile.clothing = ClothTypes::FEET;
				break;
			case 4: tile.clothing = ClothTypes::FACE;
				break;
			case 5: tile.clothing = ClothTypes::HAND;
				break;
			case 6: tile.clothing = ClothTypes::BACK;
				break;
			case 7: tile.clothing = ClothTypes::MASK;
				break;
			case 8: tile.clothing = ClothTypes::NECKLACE;
				break;
			}
			break;
		case 26: // portal
			tile.BlockType = BlockTypes::DOOR;
			break;
		case 27:
			tile.BlockType = BlockTypes::CHECKPOINT;
			break;
		case 28: // piano note
			tile.BlockType = BlockTypes::BACKGROUND;
			break;
		case 41:
			tile.BlockType = BlockTypes::WEATHER;
			break;
		case 34: // bulletin boardd
			tile.BlockType = BlockTypes::BULLETIN_BOARD;
			break;
		case 107: // ances
			tile.BlockType = BlockTypes::CLOTHING;
			tile.clothing = ClothTypes::ANCESTRAL;
			break;
		default:
			break;

		}
		// -----------------
		itemDefs.push_back(tile);
	}
	GetItemDescriptions();
}
