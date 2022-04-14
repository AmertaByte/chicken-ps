#include <iostream>
#include "Playmods.hh"
#include <chrono>

long long Play_Mods::Get_Current_Time() {
	return (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}

void Play_Mods::Revoke(ENetPeer* p_, long long int playmod_time) {
	// coming soon
}