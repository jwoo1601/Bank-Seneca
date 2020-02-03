#include "Packets.h"
#include "Logger.h"

// PacketResponse Interface //

#if SIDE == SERVER
PacketResponse::PacketResponse(EPacketResponse response, const std::string &message, unsigned requestHeader)
	: m_response(response),
	  m_message(message),
	  m_requestHeader(requestHeader)
{

}
#endif

void PacketResponse::WriteTo(Buffer &buffer) const
{
	buffer << m_response << m_message << m_requestHeader;
}

void PacketResponse::ReadFrom(Buffer &buffer)
{
	buffer >> m_response >> m_message >> m_requestHeader;
}

std::string PacketResponse::ToString() const
{
	char buffer[1024];
	const char *responseString = (m_response == EPacketResponse::Okay ? "Okay" : "Bad");

	sprintf(buffer, "PacketResponse { requestHeader: %d, response: %s, message: %s }", m_requestHeader, responseString, m_message.c_str());

	return std::string(buffer);
}

// PacketHandshake Interface //

void PacketHandshake::WriteTo(Buffer &buffer) const
{
	buffer << PACKET_VERSION;
}

void PacketHandshake::ReadFrom(Buffer &buffer)
{
	unsigned remotePacketVersion;
	buffer >> remotePacketVersion;

	if (remotePacketVersion == PACKET_VERSION)
	{
		m_bCompatible = true;
	}
}

std::string PacketHandshake::ToString() const
{
	char buffer[1024];
	sprintf(buffer, "PacketHandshake { isCompatible: %s }", (m_bCompatible? "true" : "false"));

	return std::string(buffer);
}

// PacketEndConnection Interface //
void PacketEndConnection::WriteTo(Buffer &buffer) const
{

}

void PacketEndConnection::ReadFrom(Buffer &buffer)
{

}

std::string PacketEndConnection::ToString() const
{
	char buffer[1024];
	sprintf(buffer, "PacketEndConnection { }");

	return std::string(buffer);
}

// PacketAuthenticate Interface //

#if SIDE == CLIENT
PacketAuthenticate::PacketAuthenticate(const std::string &username, const std::string &password)
	: m_username(username),
	  m_password(password)
{

}
#endif

void PacketAuthenticate::WriteTo(Buffer &buffer) const
{
	buffer << m_username << m_password;
}

void PacketAuthenticate::ReadFrom(Buffer &buffer)
{
	buffer >> m_username >> m_password;
}

std::string PacketAuthenticate::ToString() const
{
	char buffer[1024];
	sprintf(buffer, "PacketAuthenticate { username: %s, password: %s }", m_username.c_str(), m_password.c_str());

	return std::string(buffer);
}

// PacketSignup Interface //

#if SIDE == CLIENT
PacketSignup::PacketSignup(const std::string &username, const std::string &password)
	: m_username(username),
	m_password(password)
{

}
#endif

void PacketSignup::WriteTo(Buffer &buffer) const
{
	buffer << m_username << m_password;
}

void PacketSignup::ReadFrom(Buffer &buffer)
{
	buffer >> m_username >> m_password;
}

std::string PacketSignup::ToString() const
{
	char buffer[1024];
	sprintf(buffer, "PacketSignup { username: %s, password: %s }", m_username.c_str(), m_password.c_str());

	return std::string(buffer);
}

// PacketSafeAuthenticate Interface //

#if SIDE == CLIENT
PacketSafeAuthenticate::PacketSafeAuthenticate(const std::string &username, const std::string &password, int sessionKey)
	: m_username(username),
	  m_password(password),
	  m_sessionKey(sessionKey)
{

}
#endif

void PacketSafeAuthenticate::WriteTo(Buffer &buffer) const
{
	buffer << m_username << m_password << m_sessionKey;
}

void PacketSafeAuthenticate::ReadFrom(Buffer &buffer)
{
	buffer >> m_username >> m_password >> m_sessionKey;
}

std::string PacketSafeAuthenticate::ToString() const
{
	char buffer[1024];
	sprintf(buffer, "PacketSafeAuthenticate { sessionKey: %d, username: %s, password: %s }", m_sessionKey, m_username.c_str(), m_password.c_str());

	return std::string(buffer);
}