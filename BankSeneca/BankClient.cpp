#include "BankClient.h"

#if SIDE == CLIENT

#include <cstdio>

#include "NetConnection.h"
#include "Socket.h"

BankClient::BankClient(const char *ipv4Address, const char *port)
	: m_bInitialized(false),
	  m_bRunning(false),
	  m_bShouldStop(false),
	  m_ipv4(ipv4Address),
	  m_port(port)
{
	m_connection = new NetConnection(*this, ipv4Address, port);
	m_connection->SetEventListener(this);
}

BankClient::~BankClient()
{
	Finalize();

	if (m_connection)
	{
		delete m_connection;
	}
}

bool BankClient::Initialize()
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

		m_connection->Initialize();
		if (m_connection->IsInitialized())
		{
			m_bInitialized = true;
		}
	}

	return m_bInitialized;
}

void BankClient::Finalize()
{
	if (m_bInitialized)
	{
		Stop();
		m_connection->Finalize();

		WSACleanup();

		m_bInitialized = false;
	}
}

void BankClient::Run()
{
	if (m_bInitialized && !m_bRunning)
	{
		m_connection->Connect();
		if (m_connection->IsConnected())
		{
			RunIOLoop();

			m_connection->SendPacket(PacketHandshake());

			m_bRunning = true;
		}
	}
}

void BankClient::Stop()
{
	if (m_bRunning)
	{
		m_bShouldStop = true;
		m_ioThread->join();
		m_bShouldStop = false;

		delete m_ioThread;

		m_connection->Disconnect();

		m_bRunning = false;
	}
}

/* ISocketEventListener Interface */

void BankClient::OnSocketOpen(Socket *pSocket)
{

}

void BankClient::OnSocketClosed(Socket *pSocket)
{

}

void BankClient::OnSocketAccepted(Socket *pHostSocket, Socket &acceptedSocket)
{

}

void BankClient::OnSocketDataSent(Socket *pSocket, const Buffer &sentData)
{

}

void BankClient::OnSocketDataReceived(Socket *pSocket, Buffer &receivedData)
{

}

/* INetConnectionEventListener Interface */
void BankClient::OnNetConnected(NetConnection *pConnection)
{

}

void BankClient::OnNetDisconnected(NetConnection *pConnection)
{

}

void BankClient::OnPacketSent(NetConnection *pConnection, const IPacket &sentPacket)
{

}

void BankClient::OnPacketReceived(NetConnection *pConnection, IPacket &receivedPacket)
{
	
}

// Notifications
void BankClient::NotifyConnectionClosed(class NetConnection &connection)
{
	Stop();
}

void BankClient::NotifyConnectionEstablished(class NetConnection &connection)
{
	NewLoginSession();
}

void BankClient::NotifyAuthenticationSucceeded(class NetConnection &connection)
{
	Logger::Log("Authentication Success");

	int selection;
	do
	{
		DisplayMainMenu();
		printf("> ");
		scanf("%d", &selection);

		switch (selection)
		{
		case 1:
			break;

		case 2:
			break;

		case 3:
			break;
		}
	} while (selection != 0);
}

void BankClient::NotifyAuthenticationFailed(NetConnection &connection)
{
	printf("\nLogin Fail: No matching user account!\n");

	NewLoginSession();
}

void BankClient::NotifySignupSucceeded(NetConnection &connection)
{
	printf("\nSignup Succeeded!\n");

	NewLoginSession();
}

void BankClient::NotifySignupFailed(NetConnection &connection)
{
	printf("\nSignup Fail: Username already in use!\n");

	NewLoginSession();
}

void BankClient::NewLoginSession()
{
	DisplayLoginMenu();

	bool bSelected = false;
	do
	{
		switch (SelectMenu())
		{
		case 1:
			bSelected = true;
			Signup();
			break;

		case 2:
			bSelected = true;
			Login();
			break;

		case 0:
			bSelected = true;
			Stop();
			break;
		}

	} while (!bSelected);
}

void BankClient::GetUserAccountInput(std::string &username, std::string &password)
{
	printf("Enter your username: ");
	char tempUsername[256];
	scanf("%s", tempUsername);
	username = tempUsername;

	printf("Enter your password: ");
	char tempPassword[256];
	scanf("%s", tempPassword);
	password = tempPassword;
}

void BankClient::Signup()
{
	printf( "\n------------------------------------------------\n"
		    "BankSeneca Client v1.0.0\n"
		    "------------------------------------------------\n"
		    "Sign Up\n"
		    "------------------------------------------------\n");

	std::string username, password;
	GetUserAccountInput(username, password);

	printf("------------------------------------------------\n"
		   "\nSigning Up...\n\n");

	m_connection->SendPacket(PacketSignup(username, password));
}

void BankClient::Login()
{
		printf(
			"\n------------------------------------------------\n"
			"BankSeneca Client v1.0.0\n"
			"------------------------------------------------\n"
			"Sign In\n"
			"------------------------------------------------\n"
			"Enter your username: ");

		char username[256];
		scanf("%s", username);

		printf("Enter your password: ");
		char password[256];
		scanf("%s", password);

		printf("------------------------------------------------\n"
			   "\nSigning In...\n\n");

		m_connection->SendPacket(PacketAuthenticate(username, password));
}

int BankClient::SelectMenu()
{
	int selection;
	printf("> ");
	scanf("%d", &selection);

	return selection;
}

void BankClient::DisplayLoginMenu()
{
	printf(
		"\n------------------------------------------------\n"
		"BankSeneca Client v1.0.0\n"
		"------------------------------------------------\n"
		"1. Sign up\n"
		"2. Sign in\n"
		"0. Exit\n"
		"------------------------------------------------\n");
}

void BankClient::DisplayMainMenu()
{
		printf(
			"\n------------------------------------------------\n"
			"BankSeneca Client v1.0.0\n"
			"------------------------------------------------\n"
			"Enter the task to do\n"
			"------------------------------------------------\n"
			"1. Check Balance\n"
			"2. Deposit\n"
			"3. Withdraw\n"
			"0. Exit\n"
			"------------------------------------------------\n");
}

void BankClient::RunIOLoop()
{
	if (m_bInitialized && !m_bRunning)
	{
		m_ioThread = new std::thread(std::bind(&BankClient::ProcessIOEvents, this));
	}
}

void BankClient::ProcessIOEvents()
{
	while (!m_bShouldStop)
	{
		m_connection->ReceivePacket();
	}
}

#endif