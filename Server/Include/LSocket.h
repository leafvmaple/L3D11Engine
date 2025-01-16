#pragma once

#include <winsock2.h>
#include <unknwn.h>

enum ENCODE_DECODE_MODE
{
	KSG_ENCODE_DECODE_NONE = -1,
	KSG_ENCODE_DECODE = 0,
	EASYCRYPT_ENCODE_DECODE,
	KSG_ENCODE_DECODE_SIMPLE,
	KSG_ENCODE_DECODE_DYNAMIC,
	KSG_ENCODE_DECODE_FAST
};

struct IKG_Buffer : IUnknown
{
public:
	//virtual void* GetData() = 0;
	//virtual unsigned GetSize() = 0;
	//virtual unsigned GetReserveSize() = 0;
};

struct IKG_SocketStream : IUnknown
{
	//virtual int Send(IKG_Buffer* piBuffer) = 0;
	//virtual int Recv(IKG_Buffer** piRetBuffer) = 0;
};

struct KG_SOCKET_EVENT
{
	unsigned          uEventFlag;
	IKG_SocketStream* piSocket;
};

class LSocketServerAcceptor
{
public:
	LSocketServerAcceptor();

	int Init(const char szIP[], int nPort, int nMaxAcceptEachWait, int nMaxRecvBufSize, int nMaxSendBufSize, ENCODE_DECODE_MODE EncodeDecodeMode, void* pvContext);
	void Uninit();

	int Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pRetCount);

private:
	int m_nListenSocket = 0;

	int _WaitProcessAccept(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pRetCount);
};

class LSocketConnector
{
public:
	LSocketConnector();

	IKG_SocketStream* Connect(const char szIP[], int nPort);

private:
	struct sockaddr_in m_SocketAddress;
};