#include "LSocket.h"
#include "LRelayAgency.h"

LRelayAgency::LRelayAgency()
{

}

int LRelayAgency::Init()
{
	Connect();

	return 1;
}

void LRelayAgency::Uninit()
{

}

int LRelayAgency::Connect()
{
	LSocketConnector Connector;

	//Connector.Connect("127.0.0.1", 3724);

	return 1;
}
