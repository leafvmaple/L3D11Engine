#pragma once

#include "LRelayAgency.h"
#include "LPlayerManager.h"

class LGateway
{
public:
	LGateway();

	int Init();
	void Uninit();
	void Run();

	LPlayerManager m_PlayerManager;
	LRelayAgency   m_RelayAgency;
};