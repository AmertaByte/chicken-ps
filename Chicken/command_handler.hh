#pragma once
#include "main.hh"
#include "server.hh"
#include "checking.hh"
#include "player_events.hh"
#include <chrono>
#include <string>
#include <string_view>

namespace Commands {
	namespace Creator {
		void Restart_Server(ENetPeer* p_) {
			if (not Server::Player::Has_Admin_Perm(p_)) return;
			gamepacket_t p;
			p.Insert("OnConsoleMessage");
			p.Insert("**Global System Message: `4Server Restart for update!");

			gamepacket_t p2;
			p2.Insert("OnConsoleMessage");
			p2.Insert("`4Global System Message``: ``Restarting server for update in `41 ``minute");

			gamepacket_t p3(10000);
			p3.Insert("OnConsoleMessage");
			p3.Insert("`4Global System Message``: Restarting server for update in `450 ``seconds");

			gamepacket_t p4(20000);
			p4.Insert("OnConsoleMessage");
			p4.Insert("`4Global System Message``: Restarting server for update in `440 ``seconds");

			gamepacket_t p5(30000);
			p5.Insert("OnConsoleMessage");
			p5.Insert("`4Global System Message``: Restarting server for update in `430 ``seconds");

			gamepacket_t p6(40000);
			p6.Insert("OnConsoleMessage");
			p6.Insert("`4Global System Message``: Restarting server for update in `420 ``seconds");

			gamepacket_t p7(50000);
			p7.Insert("OnConsoleMessage");
			p7.Insert("`4Global System Message``: Restarting server for update in `410 ``seconds");

			gamepacket_t p8(51000);
			p8.Insert("OnConsoleMessage");
			p8.Insert("`4Global System Message``: Restarting server for update in `49 ``seconds");

			gamepacket_t p9(52000);
			p9.Insert("OnConsoleMessage");
			p9.Insert("`4Global System Message``: Restarting server for update in `48 ``seconds");

			gamepacket_t p10(53000);
			p10.Insert("OnConsoleMessage");
			p10.Insert("`4Global System Message``: Restarting server for update in `47 ``seconds");

			gamepacket_t p11(54000);
			p11.Insert("OnConsoleMessage");
			p11.Insert("`4Global System Message``: Restarting server for update in `46 ``seconds");

			gamepacket_t p12(55000);
			p12.Insert("OnConsoleMessage");
			p12.Insert("`4Global System Message``: Restarting server for update in `45 ``seconds");

			gamepacket_t p13(56000);
			p13.Insert("OnConsoleMessage");
			p13.Insert("`4Global System Message``: Restarting server for update in `44 ``seconds");

			gamepacket_t p14(57000);
			p14.Insert("OnConsoleMessage");
			p14.Insert("`4Global System Message``: Restarting server for update in `43 ``seconds");

			gamepacket_t p15(58000);
			p15.Insert("OnConsoleMessage");
			p15.Insert("`4Global System Message``: Restarting server for update in `42 ``seconds");

			gamepacket_t p16(59000);
			p16.Insert("OnConsoleMessage");
			p16.Insert("`4Global System Message``: Restarting server for update in `41 ``seconds");

			gamepacket_t p17(60000);
			p17.Insert("OnConsoleMessage");
			p17.Insert("`4Global System  Message``: Restarting server for update in `4ZERO ``seconds! Should be back up in a minute or so. BYE!");

			ENetPeer* cp_;
			for (cp_ = server->peers;
				cp_ < &server->peers[server->peerCount];
				++cp_)
			{
				if (cp_->state != ENET_PEER_STATE_CONNECTED)
					continue;
				p.CreatePacket(cp_);
				p2.CreatePacket(cp_);
				p3.CreatePacket(cp_);
				p4.CreatePacket(cp_);
				p5.CreatePacket(cp_);
				p6.CreatePacket(cp_);
				p7.CreatePacket(cp_);
				p8.CreatePacket(cp_);
				p9.CreatePacket(cp_);
				p10.CreatePacket(cp_);
				p11.CreatePacket(cp_);
				p12.CreatePacket(cp_);
				p13.CreatePacket(cp_);
				p14.CreatePacket(cp_);
				p15.CreatePacket(cp_);
				p16.CreatePacket(cp_);
				p17.CreatePacket(cp_);
			}
			server_is_running = false;
		}
		void GetDev(ENetPeer* p_) {
			if (not Server::Player::Has_Admin_Perm(p_)) {
				Server::Variant::OnConsoleMessage(p_, "Only for creators");
				return;
			}
			pInfo(p_)->dev = 1;
			Server::Player::Save(p_);
			enet_peer_disconnect_later(p_, 0);
		}
		void Stop(ENetPeer* p_) {
			if (not Server::Player::Has_Admin_Perm(p_)) return;
			server_is_running = false;
		}
	}
	namespace Mod {
		void TrackPlayer(ENetPeer* p_, string cmd, string cache) {
			if (not Server::Player::Has_Admin_Perm(p_) or pInfo(p_)->dev == 0 or pInfo(p_)->mod == 0) return;
			string target_name = cmd.substr(13, cmd.length());
			if (Found_Symbol(cmd.substr(13, cache.length() - 13 - 1))) return;
			bool found = false;
			for (ENetPeer* cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
				if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
				string tracked_name = pInfo(cp_)->rawName;
				if (pInfo(cp_)->isInvisible) found = false;
				if (pInfo(cp_)->mod == 1 or pInfo(cp_)->dev == 1) found = false;
				transform(target_name.begin(), target_name.end(), target_name.begin(), ::tolower);
				transform(tracked_name.begin(), tracked_name.end(), tracked_name.begin(), ::tolower);
				if (target_name == tracked_name) found = true;
				if (found) Server::Variant::OnConsoleMessage(p_, fmt::format("`4[SERVER-TRACKER] `2{} is currently on `5{}", target_name, pInfo(cp_)->world));
				else Server::Variant::OnConsoleMessage(p_, fmt::format("`4[SERVER-TRACKER] Error : Cannot track {}, something is missing! `$Required Things :\n1. Target's status is online.\n2. Target is not mod and higher.\n3. Target is not invisible or hidden.", target_name));
			}
		}
		void NoClip(ENetPeer* p_) {
			pInfo(p_)->BisaTembus = true;
			Server::Visual::State::update_player_state(p_);
		}
	}
	namespace Debug {
		void Get_Local_Pos(ENetPeer* p_) {
			Server::Variant::OnConsoleMessage(p_, fmt::format("Your current position :\nX: {}\nY: {}", pInfo(p_)->x / 32, pInfo(p_)->y / 32));
		}
		void Get_State(ENetPeer* p_, string chat, string cache) {
			// Debug and get state value
			PlayerMoving data_;
			data_.packetType = 0x14;
			data_.characterState = 0x0; // animation
			data_.x = 1000;
			data_.y = 0;
			data_.punchX = 0;
			data_.punchY = 0;
			data_.XSpeed = 300;
			data_.YSpeed = 600;
			data_.netID = pInfo(p_)->netID;
			data_.plantingTree = atoi(chat.substr(7, cache.length() - 7 - 1).c_str());
			Server::Visual::Packet::send_raw(4, packPlayerMoving(&data_), 56, 0, p_, ENET_PACKET_FLAG_RELIABLE);
		}
	}
	namespace Normal {
		void Captcha_Test(ENetPeer* p_) {
			Captcha captcha;
			captcha.first_number = rand() % 10000;
			captcha.second_number = rand() % 10000;
			Server::Variant::OnDialogRequest(p_, fmt::format("set_default_color|`o\nadd_label_with_icon|big|`wAre you Human?``|left|206|\nadd_spacer|small|\nadd_textbox|What will be the sum of the following numbers|left|\nadd_textbox|{} + {}|left|\nadd_text_input|captcha_answer|Answer:||32|\nadd_button|captcha_submit|Submit|", captcha.first_number, captcha.second_number));
		}
		void Help(ENetPeer* p_) {
			if (Server::Player::Has_Admin_Perm(p_)) Server::Variant::OnConsoleMessage(p_, ">> Commands are : /help /state /getdev /mod /who /color /position /sb /stop /get /trackplayer /captchatest");
			else if (pInfo(p_)->dev == 1) Server::Variant::OnConsoleMessage(p_, ">> Commands are : /help /state /mod /who /color /position /sb /trackplayer /get /captchatest");
			else if (pInfo(p_)->mod == 1) Server::Variant::OnConsoleMessage(p_, ">> Commands are : /help /state /mod /who /color /position /sb /trackplayer /captchatest");
			else Server::Variant::OnConsoleMessage(p_, ">> Commands are : /help /state /mod /who /color /position /sb /captchatest");
		}
		void SuperBroadcast(ENetPeer* p_, string chat, string cache) {
			using namespace std::chrono;
			if (pInfo(p_)->lastSB + 45000 < (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count()) {
				pInfo(p_)->lastSB = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
			}
			else {
				Server::Variant::OnConsoleMessage(p_, "Wait a minute before using the SB command again!");
				return;
			}
			string name = pInfo(p_)->displayName;
			if (pInfo(p_)->dev == 0 or pInfo(p_)->mod == 0) {
				ENetPeer* cp_;
				for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
					if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
					if (not pInfo(cp_)->hidesb) continue;
					Server::Variant::OnConsoleMessage(cp_, fmt::format("CP:0_PL:4_OID:_CT:[SB]_ `w** `5Super-Broadcast`` from `w{}```` (in {}``) ** :`` `# {}", name, pInfo(p_)->world, chat.substr(4, cache.length() - 4 - 1)));
					Server::Variant::PlayAudio(cp_, "audio/beep.wav", 0);
				}
			}
			else {
				if (pInfo(p_)->mod == 1 and pInfo(p_)->dev == 0) {
					ENetPeer* cp_;
					for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
						if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
						if (not pInfo(cp_)->hidesb) continue;
						Server::Variant::OnConsoleMessage(cp_, fmt::format("CP:0_PL:4_OID:_CT:[SB]_ `w** `5Super-Broadcast`` from `w{}```` (in `4JAMMED!``) ** :`` `^ {}", name, chat.substr(4, cache.length() - 4 - 1)));
						Server::Variant::PlayAudio(cp_, "audio/beep.wav", 0);
					}
				}
				if (pInfo(p_)->dev == 1) {
					ENetPeer* cp_;
					for (cp_ = server->peers; cp_ < &server->peers[server->peerCount]; ++cp_) {
						if (cp_->state != ENET_PEER_STATE_CONNECTED) continue;
						if (not pInfo(cp_)->hidesb) continue;
						Server::Variant::OnConsoleMessage(cp_, fmt::format("CP:0_PL:4_OID:_CT:[SB]_ `w** `5Super-Broadcast`` from `w{}```` (in `4JAMMED!``) ** :`` `5 {}", name, chat.substr(4, cache.length() - 4 - 1)));
						Server::Variant::PlayAudio(cp_, "audio/beep.wav", 0);
					}
				}
			}
		}
		void Color(ENetPeer* p_, string chat, string cache) {
			pInfo(p_)->skinColor = atoi(chat.substr(6, cache.length() - 6 - 1).c_str());
			Server::Player::update_clothes(p_);
		}
		void Send_Who(ENetPeer* p_) {
			Server::InGame::Get_All_Players_Name_in_world(p_);
		}
		void Find(ENetPeer* p_) {
			Server::Variant::OnDialogRequest(p_, "add_label_with_icon|big|`wFind item``|left|3802|\nadd_textbox|Enter a word below to find the item|\nadd_text_input|item|Item Name||30|\nend_dialog|findid|Cancel|Find the item!|\nadd_quick_exit|\n");
		}
	}
}