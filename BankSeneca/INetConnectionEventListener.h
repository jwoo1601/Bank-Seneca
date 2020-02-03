#pragma once

class NetConnection;
class IPacket;

class INetConnectionEventListener
{
public:

	virtual void OnNetConnected(NetConnection *pConnection) = 0;
	virtual void OnNetDisconnected(NetConnection *pConnection) = 0;

	virtual void OnPacketSent(NetConnection *pConnection, const IPacket &sentPacket) = 0;
	virtual void OnPacketReceived(NetConnection *pConnection, IPacket &receivedPacket) = 0;
};