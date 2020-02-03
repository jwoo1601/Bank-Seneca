#pragma once

#include "CommonIncludes.h"
#include "ISocketEventListener.h"
#include "INetConnectionEventListener.h"

#include <thread>

class Socket;
class NetConnection;

class BankClient : public ISocketEventListener, public INetConnectionEventListener
{
public:

	BankClient(const char *ipv4Address, const char *port);
	~BankClient();

	bool Initialize();
	void Finalize();

	void Run();
	void Stop();

	// Notifications
	void NotifyConnectionClosed(NetConnection &connection);
	void NotifyConnectionEstablished(NetConnection &connection);

	void NotifyAuthenticationSucceeded(NetConnection &connection);
	void NotifyAuthenticationFailed(NetConnection &connection);

	void NotifySignupSucceeded(NetConnection &connection);
	void NotifySignupFailed(NetConnection &connection);

public:

	/* ISocketEventListener Interface */
	virtual void OnSocketOpen(Socket *pSocket) override;
	virtual void OnSocketClosed(Socket *pSocket) override;
	virtual void OnSocketAccepted(Socket *pHostSocket, Socket &acceptedSocket) override;
	virtual void OnSocketDataSent(Socket *pSocket, const Buffer &sentData) override;
	virtual void OnSocketDataReceived(Socket *pSocket, Buffer &receivedData) override;

	/* INetConnectionEventListener Interface */
	virtual void OnNetConnected(NetConnection *pConnection) override;
	virtual void OnNetDisconnected(NetConnection *pConnection) override;
	virtual void OnPacketSent(NetConnection *pConnection, const IPacket &sentPacket) override;
	virtual void OnPacketReceived(NetConnection *pConnection, IPacket &receivedPacket) override;

private:

	void GetUserAccountInput(std::string &username, std::string &password);

	void NewLoginSession();
	void Signup();
	void Login();

	int SelectMenu();
	void DisplayLoginMenu();
	void DisplayMainMenu();

	void RunIOLoop();
	void ProcessIOEvents();
	
private:
	bool m_bInitialized;
	bool m_bRunning;
	bool m_bShouldStop;

	const char *m_ipv4;
	const char *m_port;

	NetConnection *m_connection;

	std::thread *m_ioThread;
};