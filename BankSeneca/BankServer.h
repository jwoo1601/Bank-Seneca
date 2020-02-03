#pragma once

#include "CommonIncludes.h"

#if SIDE == SERVER
#include "Database.h"
#include "AuthServer.h"
#include "ISocketEventListener.h"

#include <thread>

#define SERVER_DEFAULT_PORT "33559"

class NetConnection;

/* enum class EIOEventType : unsigned char
{
	Read  = 0x01,
	Write = 0x02
};

struct IOEvent
{
	EIOEventType type;
	NetConnection *connection;
}; */

class BankServer : public ISocketEventListener
{
public:

	BankServer();
	BankServer(const char *port);
	BankServer(const char *ipv4Address, const char *port);
	~BankServer();

	bool Initialize();

	void Start();
	void Stop();

	/* ISocketEventListener Interface */
	virtual void OnSocketOpen(Socket *pSocket) override;
	virtual void OnSocketClosed(Socket *pSocket) override;
	virtual void OnSocketAccepted(Socket *pHostSocket, Socket &acceptedSocket) override;
	virtual void OnSocketDataSent(Socket *pSocket, const Buffer &sentData) override;
	virtual void OnSocketDataReceived(Socket *pSocket, Buffer &receivedData) override;

	// Notifications
	void NotifyConnectionEstablished(NetConnection &connection);
	void NotifyConnectionClosed(NetConnection &connection);

	inline bool IsRunning() const
	{
		return m_bStarted;
	}

	inline const char* GetIPv4Address() const
	{
		return m_ipv4Address;
	}

	inline const char* GetPort() const
	{
		return m_port;
	}

	inline Database& GetDatabase()
	{
		return m_database;
	}

	inline const Database& GetDatabase() const
	{
		return m_database;
	}

private:

	void CheckForUpdates();
//	void RunIOLoop();

private:
	bool           m_bInitialized;
	bool           m_bStarted;
	volatile bool  m_bStopping;

	const char    *m_ipv4Address;
	const char    *m_port;

	AuthServer     m_authServer;
	Database       m_database;

	std::thread   *m_ioThread;
//	size_t         m_numIoThreads;

	NetConnection *m_connections[256];
	size_t         m_numConnections;

	BankServer(const BankServer &) = delete;
	BankServer(BankServer &&) = delete;
	BankServer& operator=(const BankServer &) = delete;
};

#endif