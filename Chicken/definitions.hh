#pragma once
#include <string>
using namespace std;

enum BlockTypes {
	FOREGROUND,
	BACKGROUND,
	CONSUMABLE,
	SEED,
	PAIN_BLOCK,
	BEDROCK,
	PROVIDER,
	MAIN_DOOR,
	SIGN,
	GEM,
	DOOR,
	CLOTHING,
	FIST,
	WRENCH,
	CHECKPOINT,
	LOCK,
	GATEWAY,
	TREASURE,
	WEATHER,
	TRAMPOLINE,
	TOGGLE_FOREGROUND,
	SWITCH_BLOCK,
	SFX_FOREGROUND,
	RANDOM_BLOCK,
	PORTAL,
	PLATFORM,
	MAILBOX,
	MAGIC_EGG,
	GEMS,
	DEADLY,
	CHEST,
	FACTION,
	BULLETIN_BOARD,
	BOUNCY,
	ANIM_FOREGROUND,
	COMPONENT,
	UNKNOWN
};
enum ClothTypes {
	HAIR,
	SHIRT,
	PANTS,
	FEET,
	FACE,
	HAND,
	BACK,
	MASK,
	ANCESTRAL,
	NECKLACE,
	NONE
};

struct ItemDefinition {
	int id;
	int item;

	unsigned char editableType = 0;
	unsigned char itemCategory = 0;
	unsigned char actionType = 0;
	unsigned char hitSoundType = 0;

	string name;

	string texture = "";
	int textureHash = 0;
	unsigned char itemKind = 0;
	int val1;
	unsigned char textureX = 0;
	unsigned char textureY = 0;
	unsigned char spreadType = 0;
	unsigned char isStripeyWallpaper = 0;
	unsigned char collisionType = 0;

	unsigned char breakHits = 0;

	int dropChance = 0;
	unsigned char ClothingType = 0;
	BlockTypes BlockType;
	int growTime;
	ClothTypes clothing;
	int rarity;
	string effect = "(Mod removed)";
	string effects = "(Mod added)";
	unsigned char maxAmount = 0;
	string extraFile = "";
	int extraFileHash = 0;
	int audioVolume = 0;
	string petName = "";
	string petPrefix = "";
	string petSuffix = "";
	string petAbility = "";
	unsigned	char seedBase = 0;
	unsigned	char seedOverlay = 0;
	unsigned	char treeBase = 0;
	unsigned	char treeLeaves = 0;
	int seedColor = 0;
	int seedOverlayColor = 0;
	bool isMultiFace = false;
	short val2;
	short isRayman = 0;
	string extraOptions = "";
	string texture2 = "";
	string extraOptions2 = "";
	string punchOptions = "";
	string description = "Nothing to see.";
};

struct PunchDefinition {
	int id;
	int pid;
};

vector<ItemDefinition> itemDefs;
vector<PunchDefinition> punchDefs;
ItemDefinition GetItemDef(int id)
{
	if (id < (int)itemDefs.size() and id > -1) return itemDefs.at(id);
	return itemDefs.at(0);
}
ItemDefinition getItemDef(int id)
{
	if (id < (int)itemDefs.size() and id > -1) return itemDefs.at(id);
	throw 0;
	return itemDefs.at(0);
}

PunchDefinition getPunchDef(int id)
{
	if (id < (int)punchDefs.size() and id > -1) return punchDefs.at(id);
	throw 0;
	return punchDefs.at(0);
}