#pragma once

class LGateway;

class LRelayAgency
{
public:
	LRelayAgency();

	int Init();
	void Uninit();
	int Connect();

	LGateway* m_pGateway = nullptr;
};