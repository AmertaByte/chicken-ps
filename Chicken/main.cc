#include <iostream>
#include <string>
#include <stdio.h>
#include "main.hh"
#include "ServerDB.hh"
#include "Visual.hh"
#include <fstream>
#include <sstream>
#include <Windows.h>
#include "stdafx.h"
#include "Console.hh"
#include "lock.hh"
#include "server.hh"
#include "gamepackets.hh"
#include "definitions.hh"
#include "player_events.hh"
#include "checking.hh"
#include "command_handler.hh"
#ifdef _WIN32
#else
#include "crypt_blowfish/crypt_gensalt.h"
#include "crypt_blowfish/crypt_gensalt.cpp"
#include "crypt_blowfish/crypt_blowfish.h"
#include "crypt_blowfish/crypt_blowfish.cpp"
#include "crypt_blowfish/ow-crypt.h"
#include "crypt_blowfish/ow-crypt.cpp"
#endif
#include <chrono>
#include <thread> // TODO
#include <mutex> // TODO
string news = "set_default_color|`o\n\nadd_label_with_icon|big|`wThe Chicken Gazette``|left|5016|\n\nadd_spacer|small|\nadd_label_with_icon|small|`5Credits : `6@GrowtopiaNoobs, @Spect, @Tron, @losx, @coldtopia, @Galvin, @Tianvan, @GuckTubeYT and @GC Members!|left|732|\nadd_label_with_icon|small|`4WARNING:`` `5Worlds (and accounts)`` might be deleted at any time if database issues appear (once per day or week).|left|4|\nadd_label_with_icon|small|`4WARNING:`` `5Accounts`` are in beta, bugs may appear and they will be probably deleted often, because of new account updates, which will cause database incompatibility.|left|4|\nadd_spacer|small|\n\nadd_url_button||``Watch: `1Watch a video about GT Private Server``|NOFLAGS|https://www.youtube.com/watch?v=_3avlDDYBBY|Open link?|0|0|\nadd_url_button||``Channel: `1Visit Growtopia Noobs' channel``|NOFLAGS|https://www.youtube.com/channel/UCLXtuoBlrXFDRtFU8vPy35g|Open link?|0|0|\nadd_url_button||``Discord: `1GuckCommunity Server!``|NOFLAGS|https://bit.ly/guckproject|Open the link?|0|0|\nadd_quick_exit|\n\nend_dialog|gazette|Close||";

#ifdef _WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main()
#endif
{
	fmt::print("Growtopia Private Server (C) {} 2022\n", "Ibord");
	fmt::print("Loading the configurations!\n");
	ConfigReader();
	if (admin_perm.size() > 0) fmt::print("There are {} accessed users loaded!\n", admin_perm.size());
	else fmt::print("There are no accessed user\n");
	worldDB.get("SERVER");
	enet_initialize();
	ENetAddress hosting;
	enet_address_set_host(&hosting, "0.0.0.0");
	hosting.port = port;
	server = enet_host_create(&hosting /* the address to bind the server host to */,
		1024      /* allow up to 32 clients and/or outgoing connections */,
		10      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (server == NULL) {
		fmt::print("An error occured while trying to create enet server host\n");
		while (1);
		exit(EXIT_FAILURE);
	}
	server->checksum = enet_crc32;
	enet_host_compress_with_range_coder(server);
	fmt::print("Building items database!\n");
	ifstream loaditem("items.dat");
	if (loaditem.fail()) {
		fmt::print("{} not found, please put your {} in this folder.\n", "items.dat");
		while (true);
	}
	BuildDatabase();
	fmt::print("{} items are loaded\n", itemDefs.size());
	fmt::print("Database is built!\n");
	ENetEvent event;
	while (server_is_running)
		while (enet_host_service(server, &event, 1000) > 0) {
			ENetPeer* p_ = event.peer;
			switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
			{
				ENetPeer* cp_;
				int total = 0;
				for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
					if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
					if (cp_->address.host == p_->address.host) total++;
				}
				p_->data = new PlayerInfo;
				char clientConnection[16];
				enet_address_get_host_ip(&p_->address, clientConnection, 16);
				pInfo(p_)->playerip = clientConnection;
				if (total > 5) {
					send_log(p_, "`rToo many accounts logged from this ip");
					enet_peer_disconnect_later(p_, 0);
				}
				
				else Send_Server_Data(p_, 1, 0, 0);
				continue;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				int msgtype = GetMessageTypeFromPacket(event.packet);
				WorldInfo* world_ = get_player_current_world(p_);
				switch (msgtype) {
				case 2:
				{
					string cache = GetTextPointerFromPacket(event.packet);
					string chat = cache.substr(cache.find("text|") + 5, cache.length() - cache.find("text|") - 1);
					if (cache.find("action|wrench") == 0) {
						Server::Variant::OnTextOverlay(p_, "wrench not supported yet");
					}
					if (cache.find("action|setSkin") == 0) {
						if (not world_) continue;
						stringstream ss(cache);
						string to;
						int id = -1;
						string color;
						while (getline(ss, to, '\n')) {
							vector<string> a_ = explode("|", to);
							if (a_.at(0) == "color") color = a_.at(1);
							if (not cuman_ada_angka(color)) continue;
							id = atoi(color.c_str());
							if (color == "2190853119") {
								id = -2104114177;
							}
							else if (color == "2527912447") {
								id = -1767054849;
							}
							else if (color == "2864971775") {
								id = -1429995521;
							}
							else if (color == "3033464831") {
								id = -1261502465;
							}
							else if (color == "3370516479") {
								id = -924450817;
							}
						}
						pInfo(p_)->skinColor = id;
						Server::Player::update_clothes(p_);
					}
					if (cache.find("action|respawn") == 0) {
						Server::Player::Respawn(p_, false);
					}
					if (cache.find("action|growid") == 0) {
						if (pInfo(p_)->haveGrowId) continue;
						Server::Variant::OnDialogRequest(p_, "text_scaling_string|Dirttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt|\nset_default_color|`o\nadd_label_with_icon|big|`wGet a GrowID``|left|206|\nadd_spacer|small|\nadd_small_font_button|RulesAndPunishment|`4Rules & Punishment|noflags|0|0|\nadd_smalltext|By choosing a `wGrowID``, you can use a name and password to logon from any device. Your `wname`` will be shown to other players!|left|\nadd_spacer|small|\nadd_text_input|username|Name||18|\nadd_smalltext|Your `wpassword`` must contain `w8 to 18 characters, 1 letter, 1 number`` and `w1 special character: @#!$^&*.,``|left|\nadd_text_input_password|password|Password||18|\nadd_text_input_password|passwordverify|Password Verify||18|\nadd_smalltext|Your `wemail`` will only be used for account verification and support. If you enter a fake email, you can't verify your account, recover or change your password.|left|\nadd_text_input|email|Email||64|\nadd_smalltext|We will never ask you for your password or email, never share it with anyone!|left|\nadd_smalltext|Your `wDiscord ID `owill be used for secondary verification if you lost access to your `wemail address`o! Please enter in such format: `wdiscordname#tag `oExample Ibord#1137. Your `wDiscord Tag `ocan be found in your `wDiscord account settings`o. if you don't have a discord account, you can fill in this column anything like `wYour GrowID#Random Number `o[Bobtickle#9999].|\nadd_text_input|discord|Discord||100|\nend_dialog|register|Cancel|Get My GrowID!|\n");
					}
					if (cache.find("action|store\nlocation|gem") == 0)
					{
						string text1 = "set_description_text|Welcome to the `2Growtopia Store``! Select the item you'd like more info on.`o `wWant to get `5Supporter`` status? Any Gem purchase (or `57,000`` Gems earned with free `5Tapjoy`` offers) will make you one. You'll get new skin colors, the `5Recycle`` tool to convert unwanted items into Gems, and more bonuses!";
						string text2 = "|enable_tabs|1";
						string text3 = "|\nadd_tab_button|main_menu|Home|interface/large/btn_shop2.rttex||1|0|0|0||||-1|-1||||";;
						string text4 = "|\nadd_tab_button|locks_menu|Locks And Stuff|interface/large/btn_shop2.rttex||0|1|0|0||||-1|-1|||";
						string text5 = "|\nadd_tab_button|itempack_menu|Item Packs|interface/large/btn_shop2.rttex||0|3|0|0||||-1|-1||||";
						string text6 = "|\nadd_tab_button|bigitems_menu|Awesome Items|interface/large/btn_shop2.rttex||0|4|0|0||||-1|-1||||";
						string text7 = "|\nadd_tab_button|weather_menu|Weather Machines|interface/large/btn_shop2.rttex|Tired of the same sunny sky?  We offer alternatives within...|0|5|0|0||||-1|-1||||";
						string text8 = "|\nadd_tab_button|token_menu|Growtoken Items|interface/large/btn_shop2.rttex||0|2|0|0||||-1|-1||||";
						string text9 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|1|";
						string text10 = "|\nadd_button|itemomonth|`oItem Of The Month``|interface/large/store_buttons/store_buttons16.rttex|September 2021:`` `9Ouroboros Charm``!<CR><CR>`2You Get:`` 1 `9Ouroboros Charm``.<CR><CR>`5Description: ``The endless loop of life and death, personified and celebrated. Is it a charm or is it a curse?|0|3|350000|0||interface/large/gui_store_button_overlays1.rttex|0|0||-1|-1||1|||||||";
						string text11 = "|\nadd_button|ads_tv|`oGrowShow TV``|interface/large/store_buttons/store_buttons30.rttex|`2You Get:`` 1 GrowShow TV.<CR><CR>`5Description:`` Growtopia's most loved gameshow now brings you its very own TV to watch up to 3 ads per day for AMAZING prizes.|0|4|50|0|||-1|-1||-1|-1||1||||||";
						string text12 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|2|";
						string text13 = "|\nadd_button|gems_glory|Road To Glory|interface/large/store_buttons/store_buttons30.rttex|rt_grope_loyalty_bundle01|0|0|0|0||interface/large/gui_store_button_overlays1.rttex|0|0|/interface/large/gui_shop_buybanner.rttex|1|0|`2You Get:`` Road To Glory and 100k Gems Instantly.<CR>`5Description:`` Earn Gem rewards when you level up. Every 10 levels you will get additional Gem rewards up to Level 50! Claim all rewards instantly if you are over level 50!! 1.6M Gems in total!! |1|||||||";
						string text14 = "|\nadd_button|gems_rain|It's Rainin' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_rain|1|5|0|0|||-1|-1||-1|-1|`2You Get:`` 200,000 Gems, 2 Growtoken, and 1 Megaphone.<CR><CR>`5Description:`` All the gems you could ever want and more plus 2 Growtokens and a Megaphone to tell the whole world about it.|1|||||||";
						string text15 = "|\nadd_button|gems_fountain|Gem Fountain|interface/large/store_buttons/store_buttons2.rttex|rt_grope_gem_fountain|0|2|0|0|||-1|-1||-1|-1|`2You Get:`` 90,000 Gems and 1 Growtoken.<CR><CR>`5Description:`` Get a pile of gems to shop to your hearts desire and 1 Growtoken.|1|||||||";
						string text16 = "|\nadd_button|gems_chest|Chest o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_chest|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 30,000 Gems.<CR><CR>`5Description:`` Get a chest containing gems.|1|||||||";
						string text17 = "|\nadd_button|gems_bag|Bag o' Gems|interface/large/store_buttons/store_buttons.rttex|rt_grope_gem_bag|1|0|0|0|||-1|-1||-1|-1|`2You Get:`` 14,000 Gems.<CR><CR>`5Description:`` Get a small bag of gems.|1|||||||";
						string text18 = "|\nadd_button|tapjoy|Earn Free Gems|interface/large/store_buttons/store_buttons.rttex||1|2|0|0|||-1|-1||-1|-1||1|||||||";
						string text19 = "|\nadd_banner|interface/large/gui_shop_featured_header.rttex|0|3|";
						string text20 = "|\nadd_button|365d|`o1-Year Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle02|0|5|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 1-Year Subscription Token and 25 Growtokens.<CR><CR>`5Description:`` One full year of special treatment AND 25 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1||||||";
						string text21 = "|\nadd_button|30d|`o30-Day Subscription Token``|interface/large/store_buttons/store_buttons22.rttex|rt_grope_subs_bundle01|0|4|0|0|||-1|-1||-1|-1|`2You Get:`` 1x 30-Day Free Subscription Token and 2 Growtokens.<CR><CR>`5Description:`` 30 full days of special treatment AND 2 Growtokens upfront! You'll get 70 season tokens (as long as there's a seasonal clash running), and 2500 gems every day and a chance of doubling any XP earned, growtime reduction on all seeds planted and Exclusive Skins!|1||||||";
						string text22 = "|\nadd_button|video_tapjoy|Watch Videos For Gems|interface/large/store_buttons/store_buttons29.rttex||0|1|0|0|1/5 VIDEOS WATCHED||-1|-1||-1|-1||1||||||";
						Server::Variant::OnStoreRequest(p_, text1 + text2 + text3 + text4 + text5 + text6 + text7 + text8 + text9 + text10 + text11 + text12 + text13 + text14 + text15 + text16 + text17 + text18 + text19 + text20 + text21 + text22);
					}
					if (cache.find("action|dialog_return") == 0) {
						stringstream ss(cache);
						string to;
						bool ShowRegisterDialog, captcha_dialog, find_dialog = false;
						string user, pass, passverify, discord, email = "";
						string clicked_button = "";
						string answer, item_find = "";
						while (getline(ss, to, '\n')) {
							vector<string> a_ = explode("|", to);
							if (a_.size() == 2) {
								if (a_.at(0) == "buttonClicked") clicked_button = a_.at(1);
								if (a_.at(0) == "dialog_name" and a_.at(1) == "findid") find_dialog = true;
								if (a_.at(0) == "dialog_name" and a_.at(1) == "register") ShowRegisterDialog = true;
								if (a_.at(0) == "dialog_name" and a_.at(1) == "captcha_submit") captcha_dialog = true;
								if (captcha_dialog) {
									if (a_.at(0) == "captcha_answer") answer = a_.at(1);
								}
								if (find_dialog) {
									if (a_.at(0) == "item") item_find = a_.at(1);
								}
								if (ShowRegisterDialog) {
									if (a_.at(0) == "username") user = a_.at(1);
									if (a_.at(0) == "password") pass = a_.at(1);
									if (a_.at(0) == "passwordverify") passverify = a_.at(1);
									if (a_.at(0) == "email") email = a_.at(1);
									if (a_.at(0) == "discord") discord = a_.at(1);
								}
							}
						}
						if (captcha_dialog) {
							fmt::print("Captcha executed\n");
							Captcha captcha;
							check_captcha(p_, atoi(answer.c_str()), captcha);
						}
						if (find_dialog and clicked_button.substr(0, 4) == "tool") {
							int item = atoi(clicked_button.substr(4, clicked_button.length() - 4).c_str());
							string id = clicked_button.substr(4, clicked_button.length() - 4).c_str();
							send_log(p_, fmt::format("`5 `oItem `w{} `o(`w{}`o) With rarity : `o(`w{}`o) `ohas been `2added `oto your inventory", getItemDef(item).name, id, getItemDef(item).rarity));
							bool c = true;
							Server::Inventory::AddItem(item, 200, p_, c);
						}
						else if (find_dialog) {
							string itemLower2;
							vector<ItemDefinition> itemDefsfind;
							for (char c : item_find) if (c < 0x20 || c>0x7A) goto SKIPFind;
							if (item_find.length() < 3) goto SKIPFind3;
							for (const ItemDefinition& item : itemDefs)
							{
								string itemLower;
								for (char c : item.name) if (c < 0x20 || c>0x7A) goto SKIPFind2;
								if (!(item.id % 2 == 0)) goto SKIPFind2;
								itemLower2 = item.name;
								std::transform(itemLower2.begin(), itemLower2.end(), itemLower2.begin(), ::tolower);
								if (itemLower2.find(itemLower) != std::string::npos) {
									itemDefsfind.push_back(item);
								}
							SKIPFind2:;
							}
						SKIPFind3:;
							string listMiddle = "";
							string listFull = "";

							for (const ItemDefinition& item : itemDefsfind) {
								string name = item.name;
								transform(name.begin(), name.end(), name.begin(), ::tolower);
								transform(item_find.begin(), item_find.end(), item_find.begin(), ::tolower);
								if (name.find(item_find) != string::npos) listMiddle += fmt::format("add_button_with_icon|tool{}|`${}``|left|{}||\n", item.id, name, item.id);
							}
							if (item_find.length() < 3) {
								listFull = "add_textbox|`4Word is less than 3 letters!``|\nadd_spacer|small|\n";
								Server::Variant::OnDialogRequest(p_, fmt::format("add_label_with_icon|big|`wFind item: {}``|left|3802|\nadd_spacer|small|\n{}add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n", item_find, listFull));
							}
							else if (itemDefsfind.size() == 0) {
								Server::Variant::OnDialogRequest(p_, fmt::format("add_label_with_icon|big|`wFind item: {}``|left|3802|\nadd_spacer|small|\n{}add_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\n", item_find, listFull));
							}
							else {
								Server::Variant::OnDialogRequest(p_, fmt::format("add_label_with_icon|big|`wFound item : {}``|left|6016|\nadd_spacer|small|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||20|\nend_dialog|findid|Cancel|Find the item!|\nadd_spacer|big|\n{}add_quick_exit|\n", item_find, listMiddle));
							}
						}
					SKIPFind:;
						if (ShowRegisterDialog) {
							int RegisterStatus = PlayerDatabase::Register(p_, user, pass, passverify, email, discord);
							switch (RegisterStatus) {
							case 1:
								send_log(p_, "`rAccount has been created!");
								Server::Variant::SetHasGrowID(p_, 1, user, pass);
								enet_peer_disconnect_later(p_, 0);
								break;
							case -1:
								send_log(p_, "`rNo symbol allowed");
								break;
							case -2:
								send_log(p_, "`rNo badword allowed");
								break;
							case -3:
								send_log(p_, "`rPlease input your real discord contant so we can contact you");
								break;
							case -4:
								send_log(p_, "`rPlease input your real email contant so we can contact you");
								break;
							case -5:
								send_log(p_, "`rMismatch password");
								break;
							case -6:
								send_log(p_, "`rYou need minimal 3 characters to create a growid!");
								break;
							case -7:
								send_log(p_, "`rThat username was taken by another user");
								break;
							default:
								break;
							}
						}
						if (clicked_button == "remove_clothes") {
							pInfo(p_)->cloth0 = 0;
							pInfo(p_)->cloth1 = 0;
							pInfo(p_)->cloth2 = 0;
							pInfo(p_)->cloth3 = 0;
							pInfo(p_)->cloth4 = 0;
							pInfo(p_)->cloth5 = 0;
							pInfo(p_)->cloth6 = 0;
							pInfo(p_)->cloth7 = 0;
							pInfo(p_)->cloth8 = 0;
							pInfo(p_)->cloth9 = 0;
							Server::Player::update_clothes(p_);
						}
					}
					if (cache.find("action|info") == 0) {
						Server::Variant::OnTextOverlay(p_, "`2Under Development!");
					}
					if (cache.find("action|drop\n|itemID|") == 0) {
						Server::Variant::OnTextOverlay(p_, "`2Under Development!");
					}
					if (cache.find("text|") != string::npos) {
						if (chat == "/mod" or chat == "/tembus") {
							Commands::Mod::NoClip(p_);
						}
						else if (chat == "/captchatest") {
							Commands::Normal::Captcha_Test(p_);
						}
						else if (chat.substr(0, 13) == "/trackplayer ") {
							Commands::Mod::TrackPlayer(p_, chat, cache);
						}
						else if (chat == "/position") {
							Commands::Debug::Get_Local_Pos(p_);
						}
						else if (chat.substr(0, 7) == "/state ") {
							Commands::Debug::Get_State(p_, chat, cache);
						}
						else if (chat == "/stop") {
							Commands::Creator::Stop(p_);
						}
						else if (chat == "/getdev") {
							Commands::Creator::GetDev(p_);
						}
						else if (chat == "/help") {
							Commands::Normal::Help(p_);
						}
						else if (chat.substr(0, 4) == "/sb ") {
							Commands::Normal::SuperBroadcast(p_, chat, cache);
						}
						else
							if (chat.substr(0, 7) == "/color ") {
								Commands::Normal::Color(p_, chat, cache);
							}
						if (chat == "/who") {
							Commands::Normal::Send_Who(p_);
						}
						if (chat.length() and chat[0] == '/') {
							Server::InGame::Send_Action(p_, chat, pInfo(p_)->netID);
						}
						else if (chat.length() > 0 and chat.length() <= 120) {
							Server::InGame::Send_Player_Chat(p_, pInfo(p_)->netID, chat);
						}
					}
					if (not pInfo(p_)->isIn) {
						if (itemdathash == 0) enet_peer_disconnect_later(p_, 0);
						gamepacket_t p;
						p.Insert("OnSuperMainStartAcceptLogonHrdxs47254722215a");
						p.Insert(itemdathash);
						p.Insert(cdnpath);
						p.Insert(cdn);
						p.Insert("cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster");
						p.Insert(fmt::format("proto={}|choosemusic=audio/mp3/about_theme.mp3|active_holiday=0|server_tick=226933875|clash_active=0|drop_lavacheck_faster=1|isPayingUser=0|", protocol));
						p.CreatePacket(p_);

						stringstream ss(GetTextPointerFromPacket(event.packet));
						string to;
						while (getline(ss, to, '\n')) {
							string id = to.substr(0, to.find("|"));
							string action = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);
							if (id == "tankIDName") {
								pInfo(p_)->tankIDName = action;
								pInfo(p_)->haveGrowId = true;
							}
							else if (id == "tankIDPass") {
								pInfo(p_)->tankIDPass = action;
							}
							else if (id == "requestedName") {
								pInfo(p_)->requestedName = action;
							}
							else if (id == "country") {
								pInfo(p_)->country = action;
							}
						}
						if (not pInfo(p_)->haveGrowId) {
							pInfo(p_)->hasLogon = true;
							pInfo(p_)->rawName = "";
							pInfo(p_)->displayName = PlayerDatabase::FixColor(pInfo(p_)->requestedName.substr(0, pInfo(p_)->requestedName.length() > 15 ? 15 : pInfo(p_)->requestedName.length())) + "_" + to_string(rand() % 1000);
						}
						else {
							pInfo(p_)->rawName = PlayerDatabase::getProperName(pInfo(p_)->tankIDName);
							int LoginStatus = PlayerDatabase::Login(p_, pInfo(p_)->rawName, pInfo(p_)->tankIDPass);
							switch (LoginStatus) {
							case 1:
								PlayerDatabase::Load(p_);
								if (pInfo(p_)->dev == 1) pInfo(p_)->displayName = "`6@" + pInfo(p_)->realName;
								else if (pInfo(p_)->mod == 1) pInfo(p_)->displayName = "`#@" + pInfo(p_)->realName;
								else pInfo(p_)->displayName = pInfo(p_)->realName;
								break;
							default:
								Wrong_Pass_Text(p_);
								enet_peer_disconnect_later(p_, 0);
								break;
							}
						}
						pInfo(p_)->displayName = PlayerDatabase::FixColor(pInfo(p_)->tankIDName.substr(0, pInfo(p_)->tankIDName.length() > 18 ? 18 : pInfo(p_)->tankIDName.length()));
						if (pInfo(p_)->displayName.length() < 3) pInfo(p_)->displayName = "Sus guy";
					}
					for (char c : pInfo(p_)->displayName) if (c < 0x20 or c > 0x7A) pInfo(p_)->displayName = "sus";
					pInfo(p_)->tankIDName = pInfo(p_)->realName;
					Server::Variant::SetHasGrowID(p_, pInfo(p_)->haveGrowId, pInfo(p_)->tankIDName, pInfo(p_)->tankIDPass);
					string pStr = GetTextPointerFromPacket(event.packet);
					if (pStr.substr(0, 17) == "action|enter_game" and not pInfo(p_)->isIn) {
						pInfo(p_)->isIn = true;
						Server::InGame::SendLobby(p_);
						PlayerInventory inventory;
						InventoryItem item;
						item.itemCount = 1;
						item.itemID = 18;
						inventory.items.push_back(item);
						item.itemCount = 1;
						item.itemID = 32;
						inventory.items.push_back(item);
						sendInventory(p_, inventory);
						pInfo(p_)->inventory = inventory;
						
						send_log(p_, "Growtopia Private Server by Ibord");
						if (not pInfo(p_)->tankIDName.empty()) send_log(p_, fmt::format("Welcome back, {}! Server UpTime : {} hours, {} minutes, {} seconds", pInfo(p_)->realName, hour, mins, secs));
						send_log(p_, Server::Player::Has_Admin_Perm(p_) ? "You have an access to get everything!" : "Welcome to Growtopia Private Server");
						gamepacket_t p;
						p.Insert("OnEmoticonDataChanged");
						p.Insert(0);
						p.Insert(u8"(wl)|ā|1&(yes)|Ă|1&(no)|ă|1&(love)|Ą|1&(oops)|ą|1&(shy)|Ć|1&(wink)|ć|1&(tongue)|Ĉ|1&(agree)|ĉ|1&(sleep)|Ċ|1&(punch)|ċ|1&(music)|Č|1&(build)|č|1&(megaphone)|Ď|1&(sigh)|ď|1&(mad)|Đ|1&(wow)|đ|1&(dance)|Ē|1&(see-no-evil)|ē|1&(bheart)|Ĕ|1&(heart)|ĕ|1&(grow)|Ė|1&(gems)|ė|1&(kiss)|Ę|1&(gtoken)|ę|1&(lol)|Ě|1&(smile)|Ā|1&(cool)|Ĝ|1&(cry)|ĝ|1&(vend)|Ğ|1&(bunny)|ě|1&(cactus)|ğ|1&(pine)|Ĥ|1&(peace)|ģ|1&(terror)|ġ|1&(troll)|Ġ|1&(evil)|Ģ|1&(fireworks)|Ħ|1&(football)|ĥ|1&(alien)|ħ|1&(party)|Ĩ|1&(pizza)|ĩ|1&(clap)|Ī|1&(song)|ī|1&(ghost)|Ĭ|1&(nuke)|ĭ|1&(halo)|Į|1&(turkey)|į|1&(gift)|İ|1&(cake)|ı|1&(heartarrow)|Ĳ|1&(lucky)|ĳ|1&(shamrock)|Ĵ|1&(grin)|😱|1&(ill)|Ķ|1&(eyes)|ķ|1&(weary)|ĸ|1&");
						p.CreatePacket(p_);
						Server::Variant::OnDialogRequest(p_, news);
					}
					if (strcmp(GetTextPointerFromPacket(event.packet), "action|refresh_item_data\n") == 0) {
						if (itemsDat != NULL) {
							ENetPacket* packet = enet_packet_create(itemsDat,
								itemsDatSize + 60,
								ENET_PACKET_FLAG_RELIABLE);
							enet_peer_send(p_, 0, packet);
							pInfo(p_)->isUpdating = true;
						}
					}
					break;
				}
				default:
					break;
				case 3:
				{
					string cch = GetTextPointerFromPacket(event.packet);
					if (cch == "action|quit") {
						Server::Player::Save(p_);
						enet_peer_disconnect_later(p_, 0);
					}
					else if (cch == "action|quit_to_exit") {
						Server::Player::Leave(p_, pInfo(p_));
						pInfo(p_)->world = "EXIT";
						Server::InGame::SendLobby(p_);
					}
					else if (cch.find("action|join_request") != string::npos) {
						try {
							fmt::print("A join_request function called\n");
							string action = explode("\n", explode("|", cch).at(2)).at(0);
							transform(action.begin(), action.end(), action.begin(), ::toupper);
							Server::World::handle(p_, action);
						}
						catch (exception& e) {
							fmt::print(e.what());
							fmt::print("\n");
						}
					}
					break;
				}
				case 4:
				{
					BYTE* tankUpdatePacket = GetStructPointerFromTankPacket(event.packet);
					if (tankUpdatePacket) {
						PlayerMoving* pMov_ = unpackPlayerMoving(tankUpdatePacket);
						if (pInfo(p_)->isInvisible) {
							pInfo(p_)->x1 = pMov_->x;
							pInfo(p_)->y1 = pMov_->y;
							pMov_->x = -1000000;
							pMov_->y = -1000000;
						}
						switch (pMov_->packetType) {
						case 0:
							pInfo(p_)->x = pMov_->x;
							pInfo(p_)->y = pMov_->y;
							pInfo(p_)->isRotatedLeft = pMov_->characterState & 0x10;
							Send_Player_Moving(p_, pMov_);
							if (not pInfo(p_)->update_clothes) {
								pInfo(p_)->update_clothes = true;
								updateAllClothes(p_);
							}
							break;
						default:
							break;
						}
						PlayerMoving* dataType_ = unpackPlayerMoving(tankUpdatePacket);
						if (dataType_->packetType == 7) {
							if (dataType_->punchX < world_->width and dataType_->punchY < world_->width) {
								if (getItemDef(world_->items[dataType_->punchX + (dataType_->punchY * world_->width)].foreground).BlockType == BlockTypes::MAIN_DOOR) {
									Server::Player::Leave(p_, pInfo(p_));
									pInfo(p_)->world = "EXIT";
									Server::InGame::SendLobby(p_);
								}
							}
						}
						if (dataType_->packetType == 10) {
							ItemDefinition def;
							try {
								def = getItemDef(pMov_->plantingTree);
							}
							catch (...) {
								goto END_CLOTHSETTER_FORCE;
							}
							switch (def.ClothingType) {
							case 0:
								if (pInfo(p_)->cloth0 == pMov_->plantingTree) {
									pInfo(p_)->cloth0 = 0;
									break;
								}
								pInfo(p_)->cloth0 = pMov_->plantingTree;
								break;
							case 1:
								if (pInfo(p_)->cloth1 == pMov_->plantingTree) {
									pInfo(p_)->cloth1 = 0;
									break;
								}
								pInfo(p_)->cloth1 = pMov_->plantingTree;
								break;
							case 2:
								if (pInfo(p_)->cloth2 == pMov_->plantingTree) {
									pInfo(p_)->cloth2 = 0;
									break;
								}
								pInfo(p_)->cloth2 = pMov_->plantingTree;
								break;
							case 3:
								if (pInfo(p_)->cloth3 == pMov_->plantingTree) {
									pInfo(p_)->cloth3 = 0;
									break;
								}
								pInfo(p_)->cloth3 = pMov_->plantingTree;
								break;
							case 4:
								if (pInfo(p_)->cloth4 == pMov_->plantingTree) {
									pInfo(p_)->cloth4 = 0;
									break;
								}
								pInfo(p_)->cloth4 = pMov_->plantingTree;
								break;
							case 5:
								if (pInfo(p_)->cloth5 == pMov_->plantingTree) {
									pInfo(p_)->cloth5 = 0;
									break;
								}
								pInfo(p_)->cloth5 = pMov_->plantingTree;
								break;
							case 6:
								if (pInfo(p_)->cloth6 == pMov_->plantingTree) {
									pInfo(p_)->cloth6 = 0;
									break;
								}
								pInfo(p_)->cloth6 = pMov_->plantingTree;
								break;
							case 7:
								if (pInfo(p_)->cloth7 == pMov_->plantingTree) {
									pInfo(p_)->cloth7 = 0;
									break;
								}
								pInfo(p_)->cloth7 = pMov_->plantingTree;
								if (getItemDef(pMov_->plantingTree).ClothingType == ClothTypes::BACK) pInfo(p_)->canDoubleJump = true;
								else pInfo(p_)->canDoubleJump = false;
								Server::Visual::State::update_player_state(p_);
								break;
							case 8:
								if (pInfo(p_)->cloth8 == pMov_->plantingTree) {
									pInfo(p_)->cloth8 = 0;
									break;
								}
								pInfo(p_)->cloth8 = pMov_->plantingTree;
								break;
							case 9:
								if (pInfo(p_)->cloth9 == pMov_->plantingTree) {
									pInfo(p_)->cloth9 = 0;
									break;
								}
								pInfo(p_)->cloth9 = pMov_->plantingTree;
								break;
							default:
								break;
							}
							Server::Player::update_clothes(p_);
						END_CLOTHSETTER_FORCE:;
						}
						if (dataType_->packetType == 11) {
							Server::Variant::OnConsoleMessage(p_, "Collect coming soon");
						}
						if (dataType_->packetType == 18) {
							Send_Player_Moving(p_, pMov_);
						}
						if (dataType_->punchX != -1 and dataType_->punchY != -1) {
							if (dataType_->packetType == 3) {
								Server::World::Update_Tile(dataType_->punchX, dataType_->punchY, dataType_->plantingTree, pInfo(p_)->netID, p_, world_);
							}
							else {
								// nothing happend
							}
						}
						delete dataType_;
						delete pMov_;
					}
					else {
					     fmt::print("bad tank packet\n");
                    }
				}
				break;
				case 5:
					break;
				case 6:
					break;
				}
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
				Server::Player::Leave(p_, pInfo(p_));
				pInfo(p_)->inventory.items.clear();
				delete pInfo(p_);
				p_->data = NULL;
			}
		}
		fmt::print("Program ended.\n");
		return 0;
}