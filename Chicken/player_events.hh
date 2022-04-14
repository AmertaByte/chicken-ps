#pragma once
#include "ServerDB.hh"
#include <fstream>
#include "checking.hh"
#include "main.hh"
#include "server.hh"
#include <chrono>
bool server_is_running = true;

using namespace std::chrono;
const auto times = system_clock::now() - system_clock::now();
const auto hour = duration_cast<hours>(times).count();
const auto mins = duration_cast<minutes>(times).count() % 60;
const auto secs = duration_cast<seconds>(times).count() % 60;

int PlayerDatabase::Register(ENetPeer* peer, string name, string pass, string passverify, string email, string discord) {
	try {
		if (Found_Symbol(name)) return -1;
		if (MulutAndaKotor(name)) return -2;
		if (discord.find("#") == string::npos and discord.length() != 0) return -3;
		if (email.find("@") == string::npos and email.length() != 0) return -4;
		if (passverify != pass) return -5;
		if (name.length() < 3) return -6;
		ifstream test("Database/players/" + PlayerDatabase::getProperName(name) + "_.json");
		if (test.is_open()) return -7;
		test.close();
		totaluids++;
		if (totaluids == 1) totaluids++;
		ofstream write("Database/players/" + PlayerDatabase::getProperName(name) + "_.json");
		if (!write.is_open()) {
			cout << GetLastError() << endl; // To print the error maybe
			_getch();
		}
		json j;
		j["inventory_size"] = 30;
		j["real_name"] = name;
		j["name"] = PlayerDatabase::getProperName(name);
		j["password"] = pass;
		j["discord"] = discord;
		j["email"] = email;
		j["userID"] = to_string(totaluids);
		j["dev"] = 0;
		j["mod"] = 0;
		j["level"] = 1;
		j["xp"] = 0;
		j["gem"] = 0;
		write << j << endl;
		write.close();
		ofstream oo("Database/inventory/" + name + "_.json");
		if (!oo.is_open()) {
			cout << GetLastError() << endl;
			_getch();
		}
		json items;
		json jjall = json::array();
		json jj;
		jj["position"] = 1;
		jj["itemid"] = 18;
		jj["count"] = 1;
		jjall.push_back(jj);

		jj["position"] = 2;
		jj["itemid"] = 32;
		jj["count"] = 1;
		jjall.push_back(jj);
		for (int i = 2; i < 200; i++)
		{
			jj["position"] = i + 1;
			jj["itemid"] = 0;
			jj["count"] = 0;
			jjall.push_back(jj);
		}
		items["items"] = jjall;
		oo << items << endl;
		oo.close();
		pInfo(peer)->userID = totaluids;
		ofstream myfile;
		myfile.open("uids.txt");
		myfile << to_string(totaluids);
		myfile.close();
		pInfo(peer)->userID = totaluids;
		return 1;
	}
	catch (exception& e) {
		cerr << e.what() << endl;
	}
}

int PlayerDatabase::Login(ENetPeer* p_, string name, string pass) {
	try {
		ifstream pro("Database/players/" + name + "_.json");
		if (pro.is_open()) {
			json j;
			pro >> j;
			string passw = j["password"];
			if (pass == passw) {
				pInfo(p_)->hasLogon = true;
				ENetPeer* cp_;
				for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
					if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
					if (cp_ == p_) continue;
					if (pInfo(cp_)->rawName == PlayerDatabase::getProperName(name)) {
						{
							Server::Variant::OnConsoleMessage(p_, "`rSomeone else was logged into this account!");
						}
						{
							Server::Variant::OnConsoleMessage(p_, "Someone else was logged into this account! He was kicked out now.");
						}
						enet_peer_disconnect_later(cp_, 0);
					}
				}
				return 1;
			}
			else return -1;
		}
		else return -2;
	}
	catch (exception& e) {
		cerr << e.what() << endl;
	}
}

void PlayerDatabase::Load(ENetPeer* peer) {
	try {
		ifstream gay("Database/players/" + pInfo(peer)->rawName + "_.json");
		json j;
		gay >> j;
		string realname = j["real_name"];
		int dev, mod, gem, lvl, xp, invsize;
		dev = j["dev"], mod = j["mod"], gem = j["gem"], lvl = j["level"], xp = j["xp"], invsize = j["inventory_size"];
		pInfo(peer)->dev = dev;
		pInfo(peer)->mod = mod;
		pInfo(peer)->level = lvl;
		pInfo(peer)->gems = gem;
		pInfo(peer)->xp = xp;
		pInfo(peer)->currentInventorySize = invsize;
		pInfo(peer)->realName = realname;
		Server::Variant::OnSetBux(peer, gem, 0);
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}
}