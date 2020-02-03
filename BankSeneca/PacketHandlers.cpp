#include "PacketHandlers.h"

#include "NetConnection.h"

#if SIDE == CLIENT
	#include "BankClient.h"
#elif SIDE == SERVER
	#include "BankServer.h"
	#include "Database.h"
#endif

#if SIDE == CLIENT
void PacketResponseHandler::HandlePacket(NetConnection &connection, const PacketType &packet)
{
	if (packet.GetResponse() == EPacketResponse::Okay)
	{
		Logger::Log("Response from server -> OKAY; RequestHeader: %d", packet.GetRequestHeader());

		switch (packet.GetRequestHeader())
		{
		case PacketHandshake::Header:
			connection.GetClient().NotifyConnectionEstablished(connection);
			break;

		case PacketAuthenticate::Header:
			connection.GetClient().NotifyAuthenticationSucceeded(connection);
			break;

		case PacketSignup::Header:
			connection.GetClient().NotifySignupSucceeded(connection);
			break;

		}
	}

	else
	{
		Logger::Log("Response from server -> BAD; RequestHeader: %d, errorMsg: %s", packet.GetRequestHeader(), packet.GetResponseMessage());

		switch (packet.GetRequestHeader())
		{
		case PacketHandshake::Header:
			connection.GetClient().NotifyConnectionClosed(connection);
			break;

		case PacketAuthenticate::Header:
			connection.GetClient().NotifyAuthenticationFailed(connection);
			break;

		case PacketSignup::Header:
			connection.GetClient().NotifySignupFailed(connection);
			break;

		}
	}
}

#elif SIDE == SERVER
void PacketHandshakeHandler::HandlePacket(NetConnection &connection, const PacketType &packet)
{
	if (packet.IsCompatible())
	{
		connection.SendPacketResponse(PacketHandshake::Header, EPacketResponse::Okay);
	}
	
	else
	{
		connection.SendPacketResponse(PacketHandshake::Header, EPacketResponse::Bad, "Packet Version Not Compatible");
		connection.Disconnect();
	}
}

void PacketAuthenticateHandler::HandlePacket(NetConnection &connection, const PacketType &packet)
{
	Database &db = connection.GetServer().GetDatabase();

	UserInfo *matchingUser = db.FindUser(packet.GetUsername());
	if (matchingUser)
	{
		if (matchingUser->MatchesPassword(packet.GetPassword()))
		{
			connection.SetAuthorized();
			connection.SendPacketResponse(PacketAuthenticate::Header, EPacketResponse::Okay);
		}

		else
		{
			connection.SendPacketResponse(PacketAuthenticate::Header, EPacketResponse::Bad, "User Password Mismatch");
		}
	}

	else
	{
		connection.SendPacketResponse(PacketAuthenticate::Header, EPacketResponse::Bad, "User Not Found");
	}
}

void PacketSignupHandler::HandlePacket(NetConnection &connection, const PacketType &packet)
{
	Database &db = connection.GetServer().GetDatabase();

	UserInfo *matchingUser = db.FindUser(packet.GetUsername());
	if (matchingUser)
	{
		connection.SendPacketResponse(PacketSignup::Header, EPacketResponse::Bad, "User Already Exists");
	}

	else
	{
		db.AddUser(packet.GetUsername(), packet.GetPassword());

		connection.SendPacketResponse(PacketSignup::Header, EPacketResponse::Okay);
	}
}


void PacketSafeAuthenticateHandler::HandlePacket(NetConnection &connection, const PacketType &packet)
{
	if (ValidateSessionKey(packet.GetSessionKey()))
	{
		Database &db = connection.GetServer().GetDatabase();

		UserInfo *matchingUser = db.FindUser(packet.GetUsername());
		if (matchingUser)
		{
			if (matchingUser->MatchesPassword(packet.GetPassword()))
			{
				connection.SetAuthorized();
				connection.SendPacketResponse(PacketSafeAuthenticate::Header, EPacketResponse::Okay);
			}

			connection.SendPacketResponse(PacketSafeAuthenticate::Header, EPacketResponse::Bad, "User Password Mismatch");
		}

		else
		{
			connection.SendPacketResponse(PacketSafeAuthenticate::Header, EPacketResponse::Bad, "User Not Found");
		}
	}

	else
	{
		connection.SendPacketResponse(PacketSafeAuthenticate::Header, EPacketResponse::Bad, "Invalid Session Key");
	}
}
#endif

void PacketEndConnectionHandler::HandlePacket(NetConnection &connection, const PacketType &packet)
{
#if SIDE == CLIENT
	connection.GetClient().NotifyConnectionClosed(connection);
#elif SIDE == SERVER
	connection.GetServer().NotifyConnectionClosed(connection);
#endif
}

void DoHandlePacket(unsigned header, NetConnection &connection, const IPacket &packet)
{
	switch (header)
	{
#if SIDE == CLIENT
		HANDLE_PACKET_IMPL(Response);
#elif SIDE == SERVER
		HANDLE_PACKET_IMPL(Handshake);
		HANDLE_PACKET_IMPL(Authenticate);
		HANDLE_PACKET_IMPL(Signup);
#endif
		HANDLE_PACKET_IMPL(EndConnection);
	}
}

bool ValidateSessionKey(int key)
{
	return true;
}