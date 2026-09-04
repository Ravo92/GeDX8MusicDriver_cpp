#include "types.h"
#include "gedx8_interface.h"


void InitializeGedx8Interface()
{
	/*
		Original:

		_LAB_10010e18 = &LAB_10001000;
		_LAB_10010e1c = &PTR_LAB_1000e040;
	*/

	// Our C++ structure is already statically initialized.
	//
	// To be safe, set the table here explicitly again.
	g_Gedx8RawInterface.methodTable = &g_Gedx8MethodTable;
}


extern "C" __declspec(dllexport)
u32 __cdecl GetInterface2(Gedx8RawInterface** outInterface)
{
	if (outInterface == nullptr)
		return 0;

	*outInterface = &g_Gedx8RawInterface;

	return 1;
}