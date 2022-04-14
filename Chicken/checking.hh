#pragma once
#include "ServerDB.hh"
#include "gamepackets.hh"
#include "main.hh"

struct Captcha {
	int result = 0;
	int first_number = 0;
	int second_number = 0;
	bool passed = false;
};

void Wrong_Pass_Text(ENetPeer* peer) {
	string text = "action|log\nmsg|`4Unable to log on : `oThat `wGrowID `odoesn't seem valid, or the password is wrong, if you don't have one, press `wCancel, `oun-check '`oI have a GrowID`o'. Then click `wConnect.";
	string text3 = "action|logon_fail\n";
	string dc = "https://discord.gg/HjPaCTY2Zv";
	string url = "action|set_url\nurl|" + dc + "\nlabel|Recover Password\n";
	BYTE* data = new BYTE[5 + text.length()];
	BYTE* data3 = new BYTE[5 + text3.length()];
	BYTE* dataurl = new BYTE[5 + url.length()];
	BYTE zero = 0;
	int type = 3;
	memcpy(data, &type, 4);
	memcpy(data + 4, text.c_str(), text.length());
	memcpy(data + 4 + text.length(), &zero, 1);
	memcpy(dataurl, &type, 4);
	memcpy(dataurl + 4, url.c_str(), url.length());
	memcpy(dataurl + 4 + url.length(), &zero, 1);
	memcpy(data3, &type, 4);
	memcpy(data3 + 4, text3.c_str(), text3.length());
	memcpy(data3 + 4 + text3.length(), &zero, 1);
	ENetPacket* p = enet_packet_create(data, 5 + text.length(), ENET_PACKET_FLAG_RELIABLE);
	ENetPacket* p2 = enet_packet_create(data3, 5 + text3.length(), ENET_PACKET_FLAG_RELIABLE);
	ENetPacket* p3 = enet_packet_create(dataurl, 5 + url.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, p);
	enet_peer_send(peer, 0, p2);
	enet_peer_send(peer, 0, p3);
	delete[] data;
	delete[] dataurl;
	delete[] data3;
}
void send_text(ENetPeer* p_, string text) {
	const auto data = new BYTE[5 + text.length()];
	BYTE zero = 0;
	auto type = 3;
	memcpy(data, &type, 4);
	memcpy(data + 4, text.c_str(), text.length());
	memcpy(data + 4 + text.length(), &zero, 1);
	const auto p = enet_packet_create(data, 5 + text.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(p_, 0, p);
	delete[] data;
}

void send_log(ENetPeer* p_, string msg) {
	send_text(p_, "action|log\nmsg|" + msg);
}

string getStrUpper(string txt) {
	string ret;
	for (char c : txt) ret += toupper(c);
	return ret;
}

bool is_currently_here(ENetPeer* peer, ENetPeer* peer2) {
	return pInfo(peer)->world == pInfo(peer2)->world;
}

bool MulutAndaKotor(string name) { // Badword blacklisting credit Tron
	string x[] = { "fuck","phuck","cunt","kunt","kike","vagina","dick","fag","phag","cocksucker","masturbate","shit","cock","nigger","n1gger","nigga","niger","bitch","asshole","penis","pecker","clit","twat","dildo","queef","pussy","pussies","sibal","bobo ","gago","porn","fitta","tanga" };
	vector <string> badwords(x, x + sizeof(x) / sizeof(string));
	for (size_t i = 0; i < badwords.size(); ++i) {
		string gg = PlayerDatabase::getProperName(name);
		if (gg.find(badwords[i]) != string::npos) return true;
		else return false;
	}
}

bool Found_Symbol(string gay) {
	return gay.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos;
}

bool cuman_ada_angka(const string str) {
	return str.find_first_not_of("0123456789") == string::npos;
}

void check_captcha(ENetPeer* p_, int answer, Captcha captcha) {
	captcha.result = captcha.first_number + captcha.second_number;
	if (answer != captcha.result) captcha.passed = false;
	if (not captcha.passed) {
		cout << "Captcha is not passed!" << endl;
		gamepacket_t p;
		p.Insert("OnDialogRequest");
		p.Insert("set_default_color|`o\nadd_label_with_icon|big|`wAre you Human?``|left|206|\nadd_spacer|small|\nadd_textbox|What will be the sum of the following numbers|left|\nadd_textbox|" + to_string(captcha.first_number) + " + " + to_string(captcha.second_number) + "|left|\nadd_text_input|captcha_answer|Answer:||32|\nadd_button|Captcha_|Submit|");
		p.CreatePacket(p_);
	}
	else {
		cout << "Captcha is passed!" << endl;
		captcha.passed = true;
	}
}