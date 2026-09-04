#pragma once

#include "types.h"
#include "gedx8musicdrv.h"

struct Gedx8DriverInstance;
struct Gedx8LoadedObject;
struct Gedx8Audiopath;

// -----------------------------------------------------------------------------
// Method pointer types
// -----------------------------------------------------------------------------

using Gedx8Entry00Fn = u8(__stdcall*)(s32 arg0, s32 arg1);

using Slot00Fn = void(__stdcall*)();

using Slot04Fn = void(__stdcall*)();

using Slot08Fn = Gedx8DriverInstance * (__stdcall*)();

using Slot0CFn = u8(__cdecl*)(Gedx8DriverInstance* instance);

using Slot10Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, const Gedx8SynthInitConfig* config);

using Slot14Fn = u8(__stdcall*)(Gedx8DriverInstance* instance);

using Slot18Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, s32 value);

using Slot1CFn = u8(__stdcall*)(Gedx8DriverInstance* instance, s32* valueOut);

using Slot20Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, s32 loadMode, const Gedx8LoadDescriptor* descriptor, Gedx8LoadedObject** loadedObjectOut, const char* basePath);

using Slot24Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, s32 value0, s32 value1);

using Slot28Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object);

using Slot2CFn = u8(__stdcall*)(Gedx8DriverInstance* instance);

using Slot30Fn = u8(__stdcall*)(Gedx8DriverInstance* instance);

struct Gedx8AudiopathConfig
{
	s32 type;          // +00: observed 3
	u32 pchannelCount; // +04: observed 0x40
};

static_assert(sizeof(Gedx8AudiopathConfig) == 0x08, "Gedx8AudiopathConfig must be 8 bytes");

using Slot34Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, const Gedx8AudiopathConfig* config, Gedx8Audiopath** audiopathOut, Gedx8LoadedObject* loadedObject);

using Slot38Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 activeState);

using Slot3CFn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 volume, s32 fadeMilliseconds);

using Slot40Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 selector, void* parameterData);

using Slot44Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32* volumeOut);

using Slot48Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 selector, void* parameterData);

using Slot4CFn = u8(__stdcall*)(Gedx8DriverInstance* instance, u32 unused0, u32 unused1);

using Slot50Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath);

struct Gedx8StartInfo
{
	u16 value00;
	u16 reserved02;
	u32 value04;
};

static_assert(sizeof(Gedx8StartInfo) == 0x08, "Gedx8StartInfo must be 8 bytes");

using Slot54Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, Gedx8LoadedObject* object, u32 flags, const Gedx8StartInfo* startInfo, s32 repeatCount, u8 downloadBeforePlay);

using Slot58Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 stopMode);

using Slot5CFn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, u8* stateOut);

using Slot60Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 mode, void* structure);

using Slot64Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 mode, void* structure);

using Slot68Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, u8* valuesOut);

using Slot6CFn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object);

using Slot70Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object);

using Slot78Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object);

using Slot7CFn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object);

using Slot80Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, const char* name);

using Slot84Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, const char* name, s32 mode, void* payload);

using Slot88Fn = Slot84Fn;

using Slot90Fn = u8(__stdcall*)(Gedx8DriverInstance* instance, Gedx8LoadedObject* object);

using Slot94Fn = u8(__stdcall*)(Gedx8DriverInstance* instance);


// -----------------------------------------------------------------------------
// Original table at 0x1000E040
// -----------------------------------------------------------------------------

struct Gedx8MethodTable
{
	Slot00Fn slot00_10001090; // +00
	Slot04Fn slot04_100010D0; // +04
	Slot08Fn slot08_100013F0; // +08
	Slot0CFn slot0C_10001630; // +0C

	Slot10Fn slot10_10001920; // +10
	Slot14Fn slot14_10001940; // +14
	Slot18Fn slot18_10001950; // +18
	Slot1CFn slot1C_10001970; // +1C

	Slot20Fn slot20_10001990; // +20
	Slot24Fn slot24_10001AB0; // +24
	Slot28Fn slot28_10001AD0; // +28
	Slot2CFn slot2C_10001B50; // +2C
	Slot30Fn slot30_10001B60; // +30
	Slot34Fn slot34_10001B70; // +34
	Slot38Fn slot38_10001CF0; // +38
	Slot3CFn slot3C_10001D10; // +3C

	Slot40Fn slot40_10001D50; // +40
	Slot44Fn slot44_10001D30; // +44
	Slot48Fn slot48_10001D70; // +48
	Slot4CFn slot4C_10001D90; // +4C

	Slot50Fn slot50_10001DA0; // +50
	Slot54Fn slot54_10001E30; // +54
	Slot58Fn slot58_10001E70; // +58
	Slot5CFn slot5C_10001E90; // +5C

	Slot60Fn slot60_10001EB0; // +60
	Slot64Fn slot64_10001EE0; // +64
	Slot68Fn slot68_10001F10; // +68
	Slot6CFn slot6C_10001F30; // +6C

	Slot70Fn slot70_10001FF0; // +70

	Slot4CFn slot74_10001D90; // +74 alias

	Slot78Fn slot78_10002010; // +78
	Slot7CFn slot7C_100020D0; // +7C
	Slot80Fn slot80_100020F0; // +80
	Slot84Fn slot84_10002110; // +84
	Slot88Fn slot88_10002180; // +88

	Slot4CFn slot8C_10001D90; // +8C alias

	Slot90Fn slot90_100021F0; // +90
	Slot94Fn slot94_100022B0; // +94
};

static_assert(sizeof(Gedx8MethodTable) == 0x98, "Size of Gedx8MethodTable is not as expected");


// -----------------------------------------------------------------------------
// Raw interface at original 0x10010E18
// -----------------------------------------------------------------------------

struct Gedx8RawInterface
{
	Gedx8Entry00Fn entry00;
	Gedx8MethodTable* methodTable;
};

static_assert(sizeof(Gedx8RawInterface) == 8, "Size of Gedx8RawInterface is not as expected");


// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

extern Gedx8MethodTable g_Gedx8MethodTable;
extern Gedx8RawInterface g_Gedx8RawInterface;


// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------

void InitializeGedx8Interface();


// -----------------------------------------------------------------------------
// DLL export
// -----------------------------------------------------------------------------

extern "C"
__declspec(dllexport)
u32 __cdecl GetInterface2(Gedx8RawInterface** outInterface);
