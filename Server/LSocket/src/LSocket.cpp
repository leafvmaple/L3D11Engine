#include "LAssert.h"
#include "LSocket.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

class CWinSockInit
{
public:
	CWinSockInit()
	{
		WORD    wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		int     nRetCode = WSAStartup(wVersionRequested, &wsaData);
		assert((!nRetCode) && "WSAStartup failed!");
	}
	~CWinSockInit()
	{
		WSACleanup();
	}
} g_WinSock; // 这个全局变量存在的意义是当模块被引用时触发一次构造函数中的WSAStartup

LSocketServerAcceptor::LSocketServerAcceptor()
{

}

static int _CloseSocket(int nSocket)
{
	struct linger lingerStruct;

	lingerStruct.l_onoff = 1;
	lingerStruct.l_linger = 0;

	setsockopt(nSocket, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct));

	return closesocket(nSocket);
}

static int _CreateListenSocket(const char szIP[], int nPort)
{
	int nResult = false;
	int nRetCode = false;
	int nListenSocket = INVALID_SOCKET;
	int nVal = 1;
	int nError = 1;

	sockaddr_in LocalAddr;

	// nListenSocket = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	nListenSocket = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	BOOL_ERROR_EXIT(nListenSocket != -1);

	nRetCode = setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nVal, sizeof(int));
	BOOL_ERROR_EXIT(nRetCode >= 0);

	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = inet_addr(szIP); //inet_pton() 
	LocalAddr.sin_port = htons(nPort);

	nRetCode = ::bind(nListenSocket, (struct sockaddr*)&LocalAddr, sizeof(LocalAddr));
	BOOL_ERROR_EXIT(nRetCode != -1);

	nRetCode = listen(nListenSocket, 128);
	BOOL_ERROR_EXIT(nRetCode >= 0);

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (nListenSocket != -1)
		{
			_CloseSocket(nListenSocket);
			nListenSocket = -1;
		}
	}
	return nListenSocket;
}

int _SetSocketNoBlock(int nSocket)
{
	int nResult = false;
	int nRetCode = 0;
	unsigned long ulOption = 1;

	nRetCode = ioctlsocket(nSocket, FIONBIO, (unsigned long*)&ulOption);
	BOOL_ERROR_EXIT(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}

int LSocketServerAcceptor::Init(const char szIP[], int nPort, int nMaxAcceptEachWait, int nMaxRecvBufSize, int nMaxSendBufSize, ENCODE_DECODE_MODE EncodeDecodeMode, void* pvContext)
{
	int nRetCode = false;

	m_nListenSocket = _CreateListenSocket(szIP, nPort);
	BOOL_ERROR_EXIT(m_nListenSocket != -1);

	nRetCode = _SetSocketNoBlock(m_nListenSocket);
	BOOL_ERROR_EXIT(nRetCode);

Exit0:
	return 1;
}


void LSocketServerAcceptor::Uninit()
{
	if (m_nListenSocket != -1)
	{
		_CloseSocket(m_nListenSocket);
		m_nListenSocket = -1;
	}
}

int LSocketServerAcceptor::Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pRetCount)
{
	int nRetCode = false;
	int nResult = false;

	nRetCode = _WaitProcessAccept(nEventCount, pEvent, pRetCount);
	BOOL_ERROR_EXIT(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int LSocketServerAcceptor::_WaitProcessAccept(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pRetCount)
{
	return 1;
}

LSocketConnector::LSocketConnector()
{

}

IKG_SocketStream* LSocketConnector::Connect(const char szIP[], int nPort)
{
	int nResult = false;
	int nRetCode = false;
	int nSocket = 0;

	IKG_SocketStream* piResult = nullptr;
	struct hostent* pHost = nullptr;

	pHost = gethostbyname(szIP); // GetAddrInfoW()
	BOOL_ERROR_EXIT(pHost);

	nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	BOOL_ERROR_EXIT(nSocket != -1);

	nRetCode = ::bind(nSocket, (struct sockaddr*)&m_SocketAddress, sizeof(sockaddr_in));
	BOOL_ERROR_EXIT(nRetCode != -1);



Exit0:
	return piResult;
}
