#pragma once

class ServerFrontEnd
{
public:

	bool CreateDevice();

	void DisposeDevice();

	void Run();

private:


private:
	bool bDeviceCreated;
};