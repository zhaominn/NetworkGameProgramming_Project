#include "Pch.h"
#include "NetworkMgr.h"

NetworkMgr::NetworkMgr()
{
}

NetworkMgr::~NetworkMgr()
{
	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
}

bool NetworkMgr::Initialize()
{
	return false;
}
