#include <windows.h>
#include "MainApp.h"
#include "CAppProcess.h"
#include "sock.h"
#include <future>
#include "NetworkCallback.h"
#include "dbg_memory.h"

#define ONLINEMODE

// 프로그램의 엔트리 포인트.
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	InitCrtCheck();

	//CrtCheck(12720);

	// 서버
#ifdef ONLINEMODE
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	// 소켓 열어서 서버 연결
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

	// 패킷 보내는 스레드를 제어하는 이벤트
	sendCompleteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (sendCompleteEvent == NULL) return 1;
	sendStartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (sendStartEvent == NULL) return 1;

	// API Create 할 때 만들어주기
	// 얘가 각각 스레드에서 함수를 호출한다.
	std::future<int> recvThread = std::async(std::launch::async, recvProc, s);
	std::future<int> sendThread = std::async(std::launch::async, sendProc, s);

#endif

	CAppProcess* pAppProcess = new CAppProcess();

	// 기본 윈도우 생성, D2D, D3D, 기타 등등 초기화
	pAppProcess->Initialize(hInstance);

	// 메인 루프
	pAppProcess->Loop();

	// 어플리케이션 종료
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