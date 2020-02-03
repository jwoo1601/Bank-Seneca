#pragma once

#include "CommonIncludes.h"

#if SIDE == SERVER
#include "Socket.h"

#include <thread>

class BankServer;

class AuthServer
{
public:

	AuthServer(BankServer &server);
	~AuthServer();

	void Initialize();
	void Finalize();
	bool IsInitialized() const;

	void Start();
	void Stop();
	bool IsRunning() const;

	inline BankServer& GetServer()
	{
		return m_server;
	}

	inline const BankServer& GetServer() const
	{
		return m_server;
	}

private:

	void BeginListen();

private:
	BankServer    &m_server;

	bool		   m_bInitialized;
	bool		   m_bStarted;
	volatile bool  m_bShouldStop;

	std::thread	  *m_listeningThread;
	Socket		   m_listeningSocket;
};

#endif