#include "BankServer.h"

#if SIDE == SERVER
#include "Logger.h"
#include "NetConnection.h"

#include <cstdio>
#include <functional>

BankServer::BankServer()
	: BankServer(SERVER_DEFAULT_PORT)
{

}

BankServer::BankServer(const char *port) : BankServer(nullptr, port)
{

}

BankServer::BankServer(const char *ipv4Address, const char *port)
	: m_bInitialized(false),
	  m_bStarted(false),
	  m_bStopping(false),
	  m_ipv4Address(ipv4Address),
	  m_port(port),
	  m_authServer(*this),
	  m_database(),
	  m_ioThread(nullptr),
	  m_numConnections(0)
{
	for (int i = 0; i < 256; i++)
	{
		m_connections[i] = nullptr;
	}
}

BankServer::~BankServer()
{
	Stop();

	if (m_ioThread)
	{
		delete m_ioThread;
	}

	for (size_t i = 0; i < m_numConnections; i++)
	{
		delete m_connections[i];
	}

	WSACleanup();
}

bool BankServer::Initialize()
{
	if (!m_bInitialized)
	{
		WSADATA wsaData;

		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult)
		{
			Logger::LogErrorCode("Failed to initialize WinSock Library", iResult);
			return false;
		}

		m_authServer.Initialize();
		if (m_authServer.IsInitialized())
		{
			return m_bInitialized = true;
		}
	}

	return false;
}

void BankServer::Start()
{
	if (m_bInitialized && !m_bStarted)
	{
		m_authServer.Start();
		m_ioThread = new std::thread(std::bind(&BankServer::CheckForUpdates, this));

		m_bStarted = true;
	}
}

void BankServer::CheckForUpdates()
{
	FD_SET readSet;
//	FD_SET writeSet;
	size_t numTotalUpdates;

	FD_ZERO(&readSet);
//	FD_ZERO(&writeSet);

	while (!m_bStopping)
	{
		for (size_t i = 0; i < m_numConnections; i++)
		{
			FD_SET(m_connections[i]->GetSocket().GetHandle(), &readSet);
			//		FD_SET(m_connections[i]->GetSocket().GetHandle(), &writeSet);
		}

		if ((numTotalUpdates = select(0, &readSet, nullptr, nullptr, nullptr)) == SOCKET_ERROR)
		{
			//				Logger::LogErrorCode("Failed to check for socket updates", WSAGetLastError());
		}

		else
		{
			for (size_t i = 0; numTotalUpdates > 0 && i < numTotalUpdates; i++)
			{
				if (FD_ISSET(m_connections[i]->GetSocket().GetHandle(), &readSet))
				{
					numTotalUpdates--;

					m_connections[i]->ReceivePacket();
				}

			/*	if (FD_ISSET(m_connections[i]->GetSocket().GetHandle(), &writeSet))
				{
					numTotalUpdates--;

					//			m_connections[i].SendPacket() ??
				} */
			}
		}
	}
}

void BankServer::Stop()
{
	if (m_bStarted)
	{
		m_bStopping = true;
		m_ioThread->join();
		m_bStopping = false;

		m_authServer.Stop();

		for (size_t i = 0; i < m_numConnections; i++)
		{
			m_connections[i]->Disconnect();
		}

		m_bStarted = false;
	}
}

/* ISocketEventListener Interface */

void BankServer::OnSocketOpen(Socket *pSocket)
{

}

void BankServer::OnSocketClosed(Socket *pSocket)
{

}

void BankServer::OnSocketAccepted(Socket *pHostSocket, Socket &acceptedSocket)
{
	m_connections[m_numConnections] = new NetConnection(*this, std::move(acceptedSocket));
	m_connections[m_numConnections]->Initialize();
	m_connections[m_numConnections]->Connect();
	m_numConnections++;
}

void BankServer::OnSocketDataSent(Socket *pSocket, const Buffer &sentData)
{

}

void BankServer::OnSocketDataReceived(Socket *pSocket, Buffer &receivedData)
{

}

// Notifications
void BankServer::NotifyConnectionEstablished(class NetConnection &connection)
{
#if SIDE == CLIENT
	Logger::Log("Server Connection Established");
#elif SIDE == SERVER
	Logger::Log("Connection Established: username=%s", connection.GetUsername());
#endif
}

void BankServer::NotifyConnectionClosed(class NetConnection &connection)
{
#if SIDE == CLIENT
	Logger::Log("Server Connection Closed");
#elif SIDE == SERVER
	Logger::Log("Connection Closed: username=%s", connection.GetUsername());
#endif
}

#endif