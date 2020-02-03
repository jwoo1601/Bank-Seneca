#include "NetConnection.h"
#include "PacketHandlers.h"

size_t const NetConnection::DefaultBufferSize = 2048U;

#if SIDE == CLIENT
	NetConnection::NetConnection(BankClient &client, const char *ipv4Address, const char *port)
		: m_client(client),
		  m_socket(ipv4Address, port),
		  m_writeBuffer(DefaultBufferSize),
		  m_readBuffer(DefaultBufferSize)
	{
		m_bInitialized = false;
		m_bConnected = false;
		m_eventListener = nullptr;
	}
	
	NetConnection::NetConnection(BankClient &client, Socket &&socket)
		: m_client(client),
		  m_socket(std::move(socket)),
		  m_writeBuffer(DefaultBufferSize),
		  m_readBuffer(DefaultBufferSize)
	{
		m_bInitialized = false;
		m_bConnected = false;
		m_eventListener = nullptr;
	}
#elif SIDE == SERVER
	NetConnection::NetConnection(BankServer &server, const char *ipv4Address, const char *port)
		: m_server(server),
		  m_socket(ipv4Address, port),
		  m_writeBuffer(DefaultBufferSize),
		  m_readBuffer(DefaultBufferSize)
	{
		m_bInitialized = false;
		m_bConnected = false;
		m_bAuthorized = false;
		m_eventListener = nullptr;
	}
	
	NetConnection::NetConnection(BankServer &server, Socket &&socket)
		: m_server(server),
		  m_socket(std::move(socket)),
		  m_writeBuffer(DefaultBufferSize),
		  m_readBuffer(DefaultBufferSize)
	{
		m_bInitialized = false;
		m_bConnected = false;
		m_bAuthorized = false;
		m_eventListener = nullptr;
	}
#endif

NetConnection::~NetConnection()
{
	Disconnect();
	Finalize();
}

void NetConnection::Initialize()
{
	if (!m_bInitialized)
	{
		m_socket.Initialize();
		if (m_socket.IsInitialized())
		{
			m_bInitialized = true;
		}
	}
}

void NetConnection::Finalize()
{
	if (m_bInitialized)
	{
		m_socket.Finalize();
		m_bInitialized = false;
	}
}

bool NetConnection::IsInitialized() const
{
	return m_bInitialized;
}

void NetConnection::Connect()
{
	if (m_bInitialized && !m_bConnected)
	{
		m_socket.Open();

		if (m_socket.IsOpen())
		{
			m_bConnected = true;
			
			if (m_eventListener)
			{
				m_eventListener->OnNetConnected(this);
			}
		}
	}
}

void NetConnection::Disconnect()
{
	if (m_bConnected)
	{
		m_socket.Close();

		m_bConnected = false;

		if (m_eventListener)
		{
			m_eventListener->OnNetDisconnected(this);
		}
	}
}

bool NetConnection::IsConnected() const
{
	return m_bConnected;
}

#if SIDE == SERVER
	void NetConnection::SendPacketResponse(unsigned requestHeader, EPacketResponse response, const std::string &message)
	{
		if (m_bConnected)
		{
			SendPacket<PacketResponse>(PacketResponse(response, message, requestHeader));
		}
	}
#endif

void NetConnection::ReceivePacket()
{
	if (m_bConnected)
	{
		m_socket.ReceiveData(m_readBuffer);

		unsigned header;
		m_readBuffer >> header;

		IPacket *pPacket = nullptr;

		switch (header)
		{
#if SIDE == CLIENT
		case PacketResponse::Header:
			pPacket = new PacketResponse();
			break;
#endif
		case PacketHandshake::Header:
			pPacket = new PacketHandshake();
			break;

		case PacketEndConnection::Header:
			pPacket = new PacketEndConnection();
			break;

#if SIDE == SERVER
		case PacketAuthenticate::Header:
			pPacket = new PacketAuthenticate();
			break;

		case PacketSignup::Header:
			pPacket = new PacketSignup();
			break;
#endif
		}

		if (pPacket)
		{
			pPacket->ReadFrom(m_readBuffer);
			m_readBuffer.Clear();
			Logger::Log("<Packet Received> %s", pPacket->ToString().c_str());

			if (m_eventListener)
			{
				m_eventListener->OnPacketReceived(this, *pPacket);
			}

			DoHandlePacket(header, *this, *pPacket);

			delete pPacket;
		}

		else
		{
			Logger::LogError("Invalid Packet Header: %d", header);
		}
	}
}

Socket& NetConnection::GetSocket()
{
	return m_socket;
}

const Socket& NetConnection::GetSocket() const
{
	return m_socket;
}

#if SIDE == CLIENT
	BankClient& NetConnection::GetClient()
	{
		return m_client;
	}

	const BankClient& NetConnection::GetClient() const
	{
		return m_client;
	}

#elif SIDE == SERVER
	BankServer& NetConnection::GetServer()
	{
		return m_server;
	}

	const BankServer& NetConnection::GetServer() const
	{
		return m_server;
	}

	bool NetConnection::IsAuthorizedClient() const
	{
		return m_bAuthorized;
	}

	void NetConnection::SetAuthorized()
	{
		if (m_bConnected && !m_bAuthorized)
		{
			m_bAuthorized = true;
		}
	}

	const std::string& NetConnection::GetUsername() const
	{
		return m_username;
	}
#endif

const Buffer& NetConnection::GetWriteBuffer() const
{
	return m_writeBuffer;
}

const Buffer& NetConnection::GetReadBuffer() const
{
	return m_readBuffer;
}

NetConnection& NetConnection::SetEventListener(INetConnectionEventListener *eventListener)
{
	m_eventListener = eventListener;

	return *this;
}

INetConnectionEventListener* NetConnection::GetEventListener()
{
	return m_eventListener;
}