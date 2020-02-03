#pragma once

class Socket;
class Buffer;

class ISocketEventListener
{
public:

	virtual void OnSocketOpen(Socket *pSocket) = 0;
	virtual void OnSocketClosed(Socket *pSocket) = 0;

	virtual void OnSocketAccepted(Socket *pHostSocket, Socket &acceptedSocket) = 0;

	virtual void OnSocketDataSent(Socket *pSocket, const Buffer &sentData) = 0;
	virtual void OnSocketDataReceived(Socket *pSocket, Buffer &receivedData) = 0;
};