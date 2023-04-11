#pragma once

#include "LSocket.h"

class LPlayerManager
{
public:
	int Init();
	void Uninit();

	void Activate();

private:
	LSocketServerAcceptor m_pSocketServer;
};