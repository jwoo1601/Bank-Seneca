#include "CommonIncludes.h"
#include "BankClient.h"
#include "BankServer.h"
#include "Logger.h"

#include "Buffer.hpp"

#include <cstring>

int main(int argc, char **argv, char **env)
{
	Logger::Iniitialize();

#if SIDE == CLIENT
//	Logger::SetLogDevice(ELogDevice::File);

	BankClient *pClientApp = nullptr;

	if (argc >= 3)
	{
		pClientApp = new BankClient(argv[1], argv[2]);
	}

	else
	{
		pClientApp = new BankClient("127.0.0.1", "33559");
//		Logger::LogError("Failed to locate the server: No IPv4 address and Port provided!");
//		return 1;
	}

	pClientApp->Initialize();
	pClientApp->Run();

	while (true);

	delete pClientApp;
#elif SIDE == SERVER
	BankServer *pServerApp = nullptr;

	if (argc >= 3)
	{
		pServerApp = new BankServer(argv[1], argv[2]);
	}

	else if (argc >= 2)
	{
		pServerApp = new BankServer(argv[1]);
	}

	else
	{
		pServerApp = new BankServer();
	}

	pServerApp->Initialize();
	pServerApp->Start();

	char buf[200];

	while (pServerApp->IsRunning())
	{
		printf("\nEnter Command: ");
		scanf("%s", buf);

		if (strcmpi(buf, "stop"))
		{
			break;
		}
	}

	pServerApp->Stop();

	delete pServerApp;

	Buffer buffer(1024);
	pServerApp->GetDatabase().SaveTo(buffer);

	FILE *fp = fopen("database.txt", "w");
	fprintf(fp, buffer.GetRawBuffer());
	fclose(fp);

#endif

	Logger::Finalize();

	return 0;
}