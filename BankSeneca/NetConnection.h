#pragma once

#include "CommonIncludes.h"
#include "Socket.h"
#include "Packets.h"
#include "INetConnectionEventListener.h"

#if SIDE == CLIENT
	#include "BankClient.h"
#elif SIDE == SERVER
	#include "BankServer.h"
#endif

#include "Logger.h"
#include "Buffer.hpp"

#include <type_traits>
#include <string>

class NetConnection
{
public:

#if SIDE == CLIENT
	NetConnection(BankClient &client, const char *ipv4Address, const char *port);
	NetConnection(BankClient &client, Socket &&socket);
#elif SIDE == SERVER
	NetConnection(BankServer &server, const char *ipv4Address, const char *port);
	NetConnection(BankServer &server, Socket &&socket);
#endif
	~NetConnection();

	NetConnection(const NetConnection &) = delete;
	NetConnection(NetConnection &&) = delete;
	NetConnection& operator=(const NetConnection &) = delete;
	NetConnection& operator=(NetConnection &&) = delete;

	void Initialize();
	void Finalize();
	bool IsInitialized() const;

	void Connect();
	void Disconnect();
	bool IsConnected() const;

	template <typename T, typename = std::enable_if_t<std::is_base_of_v<IPacket, T>>>
	void SendPacket(const T &packet);
	void ReceivePacket();

#if SIDE == SERVER
	void SendPacketResponse(unsigned requestHeader, EPacketResponse response, const std::string &message = "");
#endif

	Socket& GetSocket();
	const Socket& GetSocket() const;

#if SIDE == CLIENT
	BankClient& GetClient();
	const BankClient& GetClient() const;
#elif SIDE == SERVER
	BankServer& GetServer();
	const BankServer& GetServer() const;

	bool IsAuthorizedClient() const;
	void SetAuthorized();

	const std::string& GetUsername() const;
#endif

	const Buffer& GetWriteBuffer() const;
	const Buffer& GetReadBuffer() const;

	NetConnection& SetEventListener(INetConnectionEventListener *eventListener);
	INetConnectionEventListener* GetEventListener();

public:
	static const size_t DefaultBufferSize;

private:
	bool         m_bInitialized;
	bool         m_bConnected;
	Socket       m_socket;
	Buffer       m_writeBuffer;
	Buffer       m_readBuffer;
#if SIDE == CLIENT
	BankClient  &m_client;
#elif SIDE == SERVER
	BankServer  &m_server;
	bool         m_bAuthorized;
	std::string  m_username;
#endif

	INetConnectionEventListener *m_eventListener;
};

template <typename T, typename>
void NetConnection::SendPacket(const T &packet)
{
	if (m_bConnected)
	{
		m_writeBuffer << T::Header;
		packet.WriteTo(m_writeBuffer);

		m_socket.SendData(m_writeBuffer);
		m_writeBuffer.Clear();

		Logger::Log("<Packet Sent> %s", packet.ToString().c_str());

		if (m_eventListener)
		{
			m_eventListener->OnPacketSent(this, packet);
		}
	}
}