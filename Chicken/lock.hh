#pragma once
#include <deque>
#include "ServerDB.hh"
#include "definitions.hh"
#include "main.hh"

enum Locks {
	SMALL_LOCK = 202,
	BIG_LOCK = 204,
	HUGE_LOCK = 206,
	BUILDER_LOCK = 4994
};

struct vec2i {
	int m_x, m_y;
	vec2i(int x, int y) : m_x(x), m_y(y) {}
	bool operator==(const vec2i& other) {
		return m_x == other.m_x and m_y == other.m_y;
	}
};

bool get_current_tile(int x, int y, WorldInfo* world) {
	if (x > world->width or y > world->height) return 0;
	if (x < 0 or y < 0) return 0;
	return &world->items[x + y * world->width];
}
bool found_a_lock_in_area(WorldInfo* world_, int x, int y) {
	bool found = false;
	for (int i = 0; i < world_->width * world_->height; i++) {
		if (world_->items[i].lock_x.size() >= 1 and world_->items[i].lock_y.size() >= 1) {
			for (size_t i2 = 0; i2 < world_->items[i].lock_x.size(); i2++) {
				if (world_->items[i].lock_x[i2] == x and world_->items[i].lock_y[i2] == y) found = true;
			}
		}
	}
	return found;
}
namespace Lock {
	// Locks credit beef & losx
	void Apply(ENetPeer* p_, int x, int y, int lock_, int lock_uid, int lock_netid, WorldInfo* world_, bool reapply_) {
		try {
			int max_area;
			vector<vec2i> total;
			switch (lock_) {
			case Locks::SMALL_LOCK:
				max_area = 10;
				break;
			case Locks::BIG_LOCK:
				max_area = 48;
				break;
			case Locks::HUGE_LOCK:
				max_area = 200;
				break;
			case Locks::BUILDER_LOCK:
				max_area = 200;
				break;
			default:
				break;
			}
			vec2i start_position = { x, y };
			deque<vec2i> nodes{ start_position };

			world_->items[x + (y * world_->width)].lock_x.clear();
			world_->items[x + (y * world_->width)].lock_y.clear();

			while ((int)nodes.size() < max_area) {
				deque<vec2i> temp;
				for (const auto& node : nodes) {
					vector<vec2i> neighbours;
					if (node.m_y + 1 < world_->height) neighbours.push_back({ node.m_x, node.m_y + 1 });
					if (node.m_x + 1 < world_->width) neighbours.push_back({ node.m_x + 1, node.m_y });
					if (node.m_y - 1 >= 0) neighbours.push_back({ node.m_x, node.m_y - 1 });
					if (node.m_x - 1 >= 0) neighbours.push_back({ node.m_x - 1, node.m_y });
					if (reapply_) {
						if (node.m_x + 2 < world_->width && world_->items[(node.m_x + 1) + (node.m_y * world_->width)].foreground != 0) neighbours.push_back({ node.m_x + 2, node.m_y });
						if (node.m_x - 2 >= 0 && world_->items[(node.m_x - 1) + (node.m_y * world_->width)].foreground != 0) neighbours.push_back({ node.m_x - 2, node.m_y });
						if (node.m_y + 2 < world_->width && world_->items[node.m_x + ((node.m_y + 1) * world_->width)].foreground != 0) neighbours.push_back({ node.m_x, node.m_y + 2 });
						if (node.m_y - 2 >= 0 && world_->items[node.m_x + ((node.m_y - 1) * world_->width)].foreground != 0) neighbours.push_back({ node.m_x, node.m_y - 2 });
					}
					if (max_area != 10 and not reapply_) {
						if (node.m_x - 1 >= 0 && node.m_y - 1 >= 0) neighbours.push_back({ node.m_x - 1, node.m_y - 1 });
						if (node.m_x + 1 < world_->width && node.m_y - 1 >= 0) neighbours.push_back({ node.m_x + 1, node.m_y - 1 });
						if (node.m_x + 1 < world_->width && node.m_y + 1 < world_->height) neighbours.push_back({ node.m_x + 1, node.m_y + 1 });
						if (node.m_x - 1 >= 0 && node.m_y + 1 < world_->height) neighbours.push_back({ node.m_x - 1, node.m_y + 1 });
					}

					for (const auto& neighbour : neighbours) {
						auto tile_ = get_current_tile(x, y, world_);
						if (not tile_) continue;
						int foreground = world_->items[neighbour.m_x + (neighbour.m_y * world_->width)].foreground;
						if (find(total.begin(), total.end(), neighbour) != total.end()) continue;
						else if (found_a_lock_in_area(world_, neighbour.m_x, neighbour.m_y)) continue;
						else if (getItemDef(foreground).BlockType == BlockTypes::LOCK) continue;
						else if (reapply_ and foreground == 0) continue;
						temp.emplace_back(neighbour);
						total.emplace_back(neighbour);
						if ((int)total.size() > max_area) goto done;
					}
				}
				if (nodes.empty()) return;
				nodes.pop_front();
				if (not temp.empty()) {
					auto begin = temp.begin();
					for (auto p = temp.end() - 1; p != begin; --p) nodes.emplace_back(*p);
				}
			}
		done:;
			int size = total.size(), count = 0;
			if (size > max_area) size = max_area;
			world_->items[x + (y * world_->width)].lock_owner_id = pInfo(p_)->userID;
			PlayerMoving pmov_;
			pmov_.packetType = 0xf;
			pmov_.characterState = 0;
			pmov_.x = 0;
			pmov_.y = 0;
			pmov_.XSpeed = 0;
			pmov_.YSpeed = 0;
			pmov_.plantingTree = lock_;
			pmov_.punchX = x;
			pmov_.punchY = y;
			pmov_.netID = lock_netid;
			BYTE* pmov_p = packPlayerMoving(&pmov_);
			BYTE* packet = new BYTE[64 + max_area * 2];
			memset(packet, 0, 64 + max_area * 2);
			packet[0] = 4;
			memcpy(packet + 4, pmov_p, 56);
			delete pmov_p;
			packet[12] = max_area;
			packet[16] = 8;
			int locksz = max_area * 2;
			memcpy(packet + 56, &locksz, 4);

			uintmax_t offset = 0;
			for (const auto& pos : total) {
				if (count >= max_area) break;
				bool tile = get_current_tile(x, y, world_);
				if (tile) {
					if (pos.m_x == x and pos.m_y == y) continue;
					world_->items[x + (y * world_->width)].lock_x.push_back(pos.m_x);
					world_->items[x + (y * world_->width)].lock_y.push_back(pos.m_y);

					short lp = pos.m_x + pos.m_y * world_->width;
					memcpy(packet + world_->height + count * 2, &lp, 2);
					count += 1;
					offset += 2;
				}
			}
			ENetPacket* packet_ = enet_packet_create(packet, 64 + max_area * 2, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(p_, 0, packet_);
			delete packet;
		}
		catch (exception& e) {
			cout << e.what() << endl;
		}
	}
}