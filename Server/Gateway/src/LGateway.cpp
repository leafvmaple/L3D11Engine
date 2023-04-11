#include "LGateway.h"

LGateway::LGateway()
{

}

int LGateway::Init()
{
	m_PlayerManager.Init();

	m_RelayAgency.m_pGateway = this;
	m_RelayAgency.Init();

	return 1;
}

void LGateway::Uninit()
{
	m_RelayAgency.Uninit();
	m_PlayerManager.Uninit();
}

void LGateway::Run()
{

}
