#include "LGateway.h"

static LGateway* g_pGateway = nullptr;

int main()
{
	g_pGateway = new LGateway;
	g_pGateway->Init();

	g_pGateway->Uninit();
}