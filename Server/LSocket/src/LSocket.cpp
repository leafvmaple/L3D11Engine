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
} g_WinSock; // The significance of this global variable is to trigger WSAStartup in the constructor once when the module is referenced

static int _CloseSocket(SOCKET nSocket)
{
    linger lingerStruct{ 1, 0 };
    setsockopt(nSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&lingerStruct), sizeof(lingerStruct));
    return closesocket(nSocket);
}

static SOCKET _CreateListenSocket(const char szIP[], int nPort)
{
    SOCKET nListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (nListenSocket == INVALID_SOCKET)
        return INVALID_SOCKET;

    int nVal = 1;
    if (setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&nVal), sizeof(int)) < 0)
    {
        _CloseSocket(nListenSocket);
        return INVALID_SOCKET;
    }

    sockaddr_in LocalAddr{};
    LocalAddr.sin_family = AF_INET;
    LocalAddr.sin_addr.s_addr = inet_addr(szIP);
    LocalAddr.sin_port = htons(nPort);

    if (::bind(nListenSocket, reinterpret_cast<struct sockaddr*>(&LocalAddr), sizeof(LocalAddr)) == SOCKET_ERROR)
    {
        _CloseSocket(nListenSocket);
        return INVALID_SOCKET;
    }

    if (listen(nListenSocket, 128) == SOCKET_ERROR)
    {
        _CloseSocket(nListenSocket);
        return INVALID_SOCKET;
    }

    return nListenSocket;
}

int _SetSocketNoBlock(SOCKET nSocket)
{
    unsigned long ulOption = 1;
    return ioctlsocket(nSocket, FIONBIO, &ulOption) == 0;
}

int LSocketServerAcceptor::Init(const char szIP[], int nPort, int nMaxAcceptEachWait, int nMaxRecvBufSize, int nMaxSendBufSize, ENCODE_DECODE_MODE EncodeDecodeMode, void* pvContext)
{
    m_nListenSocket = _CreateListenSocket(szIP, nPort);
    if (m_nListenSocket == INVALID_SOCKET)
        return 0;

    if (!_SetSocketNoBlock(m_nListenSocket))
    {
        _CloseSocket(m_nListenSocket);
        m_nListenSocket = INVALID_SOCKET;
        return 0;
    }

    return 1;
}

void LSocketServerAcceptor::Uninit()
{
    if (m_nListenSocket != INVALID_SOCKET)
    {
        _CloseSocket(m_nListenSocket);
        m_nListenSocket = INVALID_SOCKET;
    }
}

int LSocketServerAcceptor::Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pRetCount)
{
    return _WaitProcessAccept(nEventCount, pEvent, pRetCount);
}

int LSocketServerAcceptor::_WaitProcessAccept(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pRetCount)
{
    return 1;
}

IKG_SocketStream* LSocketConnector::Connect(const char szIP[], int nPort)
{
    hostent* pHost = gethostbyname(szIP);
    if (!pHost)
        return nullptr;

    SOCKET nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (nSocket == INVALID_SOCKET)
        return nullptr;

    if (::bind(nSocket, reinterpret_cast<sockaddr*>(&m_SocketAddress), sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        _CloseSocket(nSocket);
        return nullptr;
    }

    return nullptr;
}
