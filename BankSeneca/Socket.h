#pragma once

#include "CommonIncludes.h"
#include "ISocketEventListener.h"

class Buffer;

class Socket
{
public:

	Socket(const char *ipv4Address, const char *port, bool bIsListenSocket = false);
	Socket(SOCKET handle, bool bIsListenSocket = false);
	Socket(const Socket &rhs) = delete;
	Socket(Socket &&rhs);
	~Socket();

	Socket& operator=(const Socket &rhs) = delete;
	Socket& operator=(Socket &&rhs);

	void Initialize();
	void Finalize();

	void Open();
	void Close();

	void Listen();

	void SendData(const Buffer &buffer);
	void ReceiveData(Buffer &buffer);

	bool IsListenable() const;
	bool IsInitialized() const;
	bool IsOpen() const;

	SOCKET GetHandle() const;

	Socket& SetEventListener(ISocketEventListener *eventListener);
	ISocketEventListener* GetEventListener() const;

private:
	bool        m_bIsListenSocket;
	bool        m_bInitialized;
	bool        m_bOpen;

	const char *m_ipv4Address;
	const char *m_port;

	SOCKET      m_handle;
	addrinfo   *m_pAddressInfo;

	ISocketEventListener *m_eventListener;
};