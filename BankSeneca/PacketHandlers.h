#pragma once

#include "Packets.h"

#include <type_traits>

class NetConnection;

template <typename T, typename = std::enable_if_t<std::is_base_of_v<IPacket, T>>>
class IPacketHandler
{
protected:
	typedef T PacketType;

public:

	virtual void HandlePacket(NetConnection &connection, const PacketType &packet) = 0;
};

#define DEFINE_PACKET_HANDLER(packetName) \
class Packet##packetName##Handler : public IPacketHandler<Packet##packetName> \
{ \
public: \
	virtual void HandlePacket(NetConnection &connection, const PacketType &packet); \
};

#if SIDE == CLIENT
	DEFINE_PACKET_HANDLER(Response);
#elif SIDE == SERVER
	DEFINE_PACKET_HANDLER(Handshake);
	DEFINE_PACKET_HANDLER(Authenticate);
	DEFINE_PACKET_HANDLER(Signup);
	DEFINE_PACKET_HANDLER(SafeAuthenticate);
#endif
	DEFINE_PACKET_HANDLER(EndConnection);


#define HANDLE_PACKET_IMPL(packetName) \
case Packet##packetName::Header: \
	Packet##packetName##Handler().HandlePacket(connection, static_cast<const Packet##packetName&>(packet)); \
	break;

void DoHandlePacket(unsigned header, NetConnection &connection, const IPacket &packet);

static bool ValidateSessionKey(int key);