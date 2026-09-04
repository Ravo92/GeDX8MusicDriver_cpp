#include <Windows.h>

#include "gedx8_interface.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		InitializeGedx8Interface();

		break;
	}

	case DLL_PROCESS_DETACH:
	{
		break;
	}
	}

	return TRUE;
}