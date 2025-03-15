#include <windows.h>
#include "MainApp.h"
#include "CAppProcess.h"
#include "sock.h"
#include <future>
#include "NetworkCallback.h"
#include "dbg_memory.h"

#define ONLINEMODE

// ���α׷��� ��Ʈ�� ����Ʈ.
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	InitCrtCheck();

	//CrtCheck(12720);

	// ����
#ifdef ONLINEMODE
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	// ���� ��� ���� ����
	SOCK* s = new SOCK();
	hostent* host = gethostbyname("project01.iptime.org"); 
	//hostent* host = gethostbyname("yoplait.iptime.org");
	//hostent* host = gethostbyname("127.0.0.1"); 

	if (!host)
	{
		host = gethostbyname("yoplait.iptime.org");
		//host = gethostbyname("project01.iptime.org");
		if (!host)
		{
			WSACleanup();
			delete s;
			return -1;
		}
	}

	for (int k = 0; host->h_addr_list[k] != NULL; k++)
		std::cout << "SERVER IP : " << inet_ntoa(*(struct in_addr*)host->h_addr_list[k]) << std::endl;

	if (!s->Connect(inet_ntoa(*(struct in_addr*)host->h_addr_list[0]), 8181))
	{
		std::cout << "Server Connect Failed" << std::endl;
		delete s;
		return -1;
	}

	// ��Ŷ ������ �����带 �����ϴ� �̺�Ʈ
	sendCompleteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (sendCompleteEvent == NULL) return 1;
	sendStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (sendStartEvent == NULL) return 1;

	// API Create �� �� ������ֱ�
	// �갡 ���� �����忡�� �Լ��� ȣ���Ѵ�.
	std::future<int> recvThread = std::async(std::launch::async, recvProc, s);
	std::future<int> sendThread = std::async(std::launch::async, sendProc, s);

#endif

	CAppProcess* pAppProcess = new CAppProcess();

	// �⺻ ������ ����, D2D, D3D, ��Ÿ ��� �ʱ�ȭ
	pAppProcess->Initialize(hInstance);

	// ���� ����
	pAppProcess->Loop();

	// ���ø����̼� ����
	pAppProcess->Finalize();

	delete pAppProcess;

#ifdef ONLINEMODE
	delete s;

	SendFunc(PROTOCOL::UNKNOWN);

	recvThread.wait();
	sendThread.wait();

	CloseHandle(sendCompleteEvent);
	CloseHandle(sendStartEvent);

	WSACleanup();
#endif

	EndCrtCheck();

	return 0;
}