#include "Socket.h"
#include "Logger.h"
#include "Buffer.hpp"
#include "BankServer.h"

Socket::Socket(const char *ipv4Address, const char *port, bool bIsListenSocket)
	: m_bIsListenSocket(bIsListenSocket),
	  m_bInitialized(false),
	  m_bOpen(false),
	  m_ipv4Address(ipv4Address),
	  m_port(port),
	  m_handle(INVALID_SOCKET),
	  m_pAddressInfo(nullptr),
	  m_eventListener(nullptr)
{

}

Socket::Socket(SOCKET handle, bool bIsListenSocket)
	: m_bIsListenSocket(bIsListenSocket),
	  m_ipv4Address(nullptr),
	  m_port(nullptr),
	  m_handle(handle),
	  m_pAddressInfo(nullptr),
	  m_eventListener(nullptr)
{
	if (handle != INVALID_SOCKET)
	{
		m_bInitialized = true;
		m_bOpen = true;
	}
}

Socket::Socket(Socket &&rhs)
{
	*this = std::move(rhs);
}

Socket::~Socket()
{
	Finalize();
}

Socket& Socket::operator=(Socket &&rhs)
{
	m_bIsListenSocket = rhs.m_bIsListenSocket;
	m_bInitialized = rhs.m_bInitialized;
	m_bOpen = rhs.m_bOpen;

	m_ipv4Address = rhs.m_ipv4Address;
	m_port = rhs.m_port;

	m_handle = rhs.m_handle;
	rhs.m_handle = INVALID_SOCKET;

	m_pAddressInfo = rhs.m_pAddressInfo;
	rhs.m_pAddressInfo = nullptr;

	m_eventListener = rhs.m_eventListener;

	return *this;
}

void Socket::Initialize()
{
	if (!m_bInitialized)
	{
		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));

		if (m_bIsListenSocket)
		{
			hints.ai_family = AF_INET;
			hints.ai_flags = AI_PASSIVE;
		}

		else
		{
			hints.ai_family = AF_UNSPEC;
		}

		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		int iResult = getaddrinfo(m_ipv4Address, m_port, &hints, &m_pAddressInfo);
		if (iResult != 0)
		{
			Logger::LogErrorCode("Failed to initialize Address Info", iResult);
			return;
		}

		m_handle = socket(m_pAddressInfo->ai_family, m_pAddressInfo->ai_socktype, m_pAddressInfo->ai_protocol);

		if (m_handle == INVALID_SOCKET)
		{
			Logger::LogErrorCode("Failed to create a listen socket for this connection", WSAGetLastError());
			return;
		}

		m_bInitialized = true;
	}
}

void Socket::Finalize()
{
	if (m_bInitialized)
	{
		if (m_bOpen)
		{
			Close();
		}

		if (m_pAddressInfo)
		{
			freeaddrinfo(m_pAddressInfo);
		}

		m_bInitialized = false;
	}
}

void Socket::Open()
{
	if (m_bInitialized && !m_bOpen)
	{
		int iResult;

		if (m_bIsListenSocket)
		{
			iResult = bind(m_handle, m_pAddressInfo->ai_addr, (int)m_pAddressInfo->ai_addrlen);
		}

		else
		{
			iResult = connect(m_handle, m_pAddressInfo->ai_addr, (int)m_pAddressInfo->ai_addrlen);
		}

		if (iResult == SOCKET_ERROR)
		{
			closesocket(m_handle);
			Logger::LogErrorCode("Failed to open a socket on the address", WSAGetLastError());
		}

		else
		{
			m_bOpen = true;
			Logger::Log("Socket Opened on address: %s:%s\n", m_ipv4Address, m_port);

			if (m_eventListener)
			{
				m_eventListener->OnSocketOpen(this);
			}
		}
	}
}

void Socket::Close()
{
	if (m_bOpen)
	{
		closesocket(m_handle);
		m_bOpen = false;

		if (m_eventListener)
		{
			m_eventListener->OnSocketClosed(this);
		}
	}
}

void Socket::Listen()
{
	if (m_bOpen)
	{
		if (listen(m_handle, SOMAXCONN) == SOCKET_ERROR)
		{
			Logger::LogErrorCode("Failed to listen on a socket", WSAGetLastError());
			Close();
		}

		else
		{
			SOCKET acceptedHandle = accept(m_handle, NULL, NULL);
			if (acceptedHandle == INVALID_SOCKET)
			{
				return;
			}

			if (m_eventListener)
			{
				Socket accepted(acceptedHandle);
				m_eventListener->OnSocketAccepted(this, accepted);
			}
		}
	}
}

void Socket::SendData(const Buffer &buffer)
{
	if (m_bOpen)
	{
		if (send(m_handle, buffer.GetRawBuffer(), buffer.GetWriteIndex(), 0) == SOCKET_ERROR)
		{
			Logger::LogErrorCode("Failed to send data in a buffer", WSAGetLastError());
		}

		else
		{
			Logger::Log("Data successfully sent through a socket --bytesSent: %d", buffer.GetWriteIndex());

			if (m_eventListener)
			{
				m_eventListener->OnSocketDataSent(this, buffer);
			}
		}
	}
}

void Socket::ReceiveData(Buffer &buffer)
{
	if (m_bOpen)
	{
		size_t bytesReceived = recv(m_handle, buffer.GetRawBuffer(), buffer.GetBufferSize(), 0);
		buffer.SetWriteIndex(bytesReceived);

		if (bytesReceived > 0)
		{
			Logger::Log("Data successfully received through a socket --bytesReceived: %d", bytesReceived);

			if (m_eventListener)
			{
				m_eventListener->OnSocketDataReceived(this, buffer);
			}
		}

		else
		{
			Logger::LogErrorCode("No data received through socket", WSAGetLastError());
			buffer.Clear();
		}
	}
}

bool Socket::IsListenable() const
{
	return m_bIsListenSocket;
}

bool Socket::IsOpen() const
{
	return m_bOpen;
}

bool Socket::IsInitialized() const
{
	return m_bInitialized;
}

SOCKET Socket::GetHandle() const
{
	return m_handle;
}

Socket& Socket::SetEventListener(ISocketEventListener *eventListener)
{
	m_eventListener = eventListener;

	return *this;
}

ISocketEventListener* Socket::GetEventListener() const
{
	return m_eventListener;
}