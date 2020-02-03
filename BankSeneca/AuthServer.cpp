#include "AuthServer.h"

#if SIDE == SERVER
#include "BankServer.h"

AuthServer::AuthServer(BankServer &server)
	: m_server(server),
	  m_bInitialized(false),
	  m_bStarted(false),
	  m_bShouldStop(false),
	  m_listeningThread(nullptr),
	  m_listeningSocket(server.GetIPv4Address(), server.GetPort(), true)
{
	m_listeningSocket.SetEventListener(&server);
}

AuthServer::~AuthServer()
{
	Finalize();
}

void AuthServer::Initialize()
{
	if (!m_bInitialized)
	{
		m_listeningSocket.Initialize();
		if (m_listeningSocket.IsInitialized())
		{
			m_bInitialized = true;
		}
	}
}

void AuthServer::Finalize()
{
	if (m_bInitialized)
	{
		Stop();

		m_listeningSocket.Finalize();
		m_bInitialized = false;
	}
}

bool AuthServer::IsInitialized() const
{
	return m_bInitialized;
}

void AuthServer::Start()
{
	if (m_bInitialized && !m_bStarted)
	{
		m_listeningSocket.Open();
		if (m_listeningSocket.IsOpen())
		{
			m_listeningThread = new std::thread(std::bind(&AuthServer::BeginListen, this));
			m_bStarted = true;
		}
	}
}

void AuthServer::Stop()
{
	if (m_bStarted)
	{
		m_bShouldStop = true;
		m_listeningThread->join();
		m_bShouldStop = false;

		delete m_listeningThread;

		m_listeningSocket.Close();

		m_bStarted = false;
	}
}

bool AuthServer::IsRunning() const
{
	return m_bStarted;
}

void AuthServer::BeginListen()
{
	while (!m_bShouldStop)
	{
		m_listeningSocket.Listen();
	}
}
#endif