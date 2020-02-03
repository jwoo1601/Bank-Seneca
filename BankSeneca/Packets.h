#pragma once

#include "CommonIncludes.h"
#include "Buffer.hpp"

#include <string>

// Packet Version
#define PACKET_VERSION 100

#define SET_PACKET_HEADER(value) \
public: \
	static const unsigned Header = value; \
	unsigned GetHeader() const { return Header; }

#define PACKET_INTERFACE(classname, value) \
public: \
	SET_PACKET_HEADER(value); \
	virtual void WriteTo(Buffer &buffer) const override; \
	virtual void ReadFrom(Buffer &buffer) override; \
	virtual std::string ToString() const override;

class IPacket
{
public:

	virtual void WriteTo(Buffer &buffer) const = 0;
	virtual void ReadFrom(Buffer &buffer) = 0;
	virtual std::string ToString() const = 0;
};

enum class EPacketResponse : unsigned char
{
	Okay = 0x01,
	Bad = 0x02
};

class PacketResponse : public IPacket
{
	PACKET_INTERFACE(PacketResponse, 0x00);

public:

#if SIDE == SERVER
	PacketResponse(EPacketResponse response, const std::string &message, unsigned requestHeader);
#endif

	inline EPacketResponse GetResponse() const
	{
		return m_response;
	}

	inline const std::string& GetResponseMessage() const
	{
		return m_message;
	}

	inline unsigned GetRequestHeader() const
	{
		return m_requestHeader;
	}

private:
	EPacketResponse m_response;
	std::string     m_message;
	unsigned        m_requestHeader;
};

class PacketHandshake : public IPacket
{
	PACKET_INTERFACE(PacketHandshake, 0x01);

public:

	inline bool IsCompatible() const
	{
		return m_bCompatible;
	}

private:
	bool m_bCompatible;
};

class PacketEndConnection : public IPacket
{
	PACKET_INTERFACE(PacketEndConnection, 0x02)
};

class PacketAuthenticate : public IPacket
{
	PACKET_INTERFACE(PacketAuthenticate, 0x03);

public:

#if SIDE == CLIENT
	PacketAuthenticate(const std::string &username, const std::string &password);
#endif

	inline const std::string& GetUsername() const
	{
		return m_username;
	}

	inline const std::string& GetPassword() const
	{
		return m_password;
	}

private:
	std::string m_username;
	std::string m_password;
};

class PacketSignup : public IPacket
{
	PACKET_INTERFACE(PacketSignup, 0x04);

public:

#if SIDE == CLIENT
	PacketSignup(const std::string &username, const std::string &password);
#endif

	inline const std::string& GetUsername() const
	{
		return m_username;
	}

	inline const std::string& GetPassword() const
	{
		return m_password;
	}

private:
	std::string m_username;
	std::string m_password;
};

class PacketSafeAuthenticate : public IPacket
{
	PACKET_INTERFACE(PacketSafeAuthenticate, 0x10);

public:

#if SIDE == CLIENT
	PacketSafeAuthenticate(const std::string &username, const std::string &password, int sessionKey);
#endif

	inline const std::string& GetUsername() const
	{
		return m_username;
	}

	inline const std::string& GetPassword() const
	{
		return m_password;
	}

	inline int GetSessionKey() const
	{
		return m_sessionKey;
	}

private:
	std::string m_username;
	std::string m_password;

	// IMPORTANT //
	int m_sessionKey;
};