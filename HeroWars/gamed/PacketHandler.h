
#ifndef _PACKET_HANDLER_H
#define _PACKET_HANDLER_H
#include <enet/enet.h>
#include "common.h"
#include "Log.h"

#include <intlib/base64.h>
#include <intlib/blowfish.h>

#include "Packets.h"
#include "Client.h"

#define RELIABLE ENET_PACKET_FLAG_RELIABLE
#define UNRELIABLE 0

#define HANDLE_ARGS ENetPeer *peer, ENetPacket *packet


class PacketHandler
{
	public:
		PacketHandler(ENetHost *server, BlowFish *blowfish);
		~PacketHandler();

		bool handlePacket(ENetPeer *peer, ENetPacket *packet);

		//Handlers
		bool handleNull(HANDLE_ARGS);
		bool handleKeyCheck(HANDLE_ARGS);
		bool handleLoadPing(HANDLE_ARGS);
		bool handleSpawn(HANDLE_ARGS);
		bool handleMap(HANDLE_ARGS);
		bool handleSynch(HANDLE_ARGS);
		bool handleGameNumber(HANDLE_ARGS);
		bool handleQueryStatus(HANDLE_ARGS);
		bool handleInit(HANDLE_ARGS);
		bool handleView(HANDLE_ARGS);
		bool handleAttentionPing(HANDLE_ARGS);

		//Tools
		void printPacket(uint8 *buf, uint32 len);
		void printLine(uint8 *buf, uint32 len);
		bool sendPacket(ENetPeer *peer, uint8 *data, uint32 length, uint8 channelNo, uint32 flag = RELIABLE);
		bool broadcastPacket(uint8 *data, uint32 length, uint8 channelNo, uint32 flag = RELIABLE);

	private:
		ENetHost *_server;
		BlowFish *_blowfish;
};

#define TOTAL_HANDLERS sizeof(table)/sizeof(PacketTable)
typedef struct PacketTable
{
	PacketCmd cmd;
	bool (PacketHandler::*handler)(HANDLE_ARGS);
}AuthHandler;

#endif