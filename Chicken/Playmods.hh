#pragma once
#include <string>
#include "enet/enet.h"

struct PlayMods {
	long long int playmod_time = 0;
	std::string playmod_user_name = "";
	int item_id = 0;
};

class Play_Mods {
	static long long Get_Current_Time();
	static void Revoke(ENetPeer* p_, long long int playmod_time);
};
