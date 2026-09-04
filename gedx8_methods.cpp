#include <Windows.h>
#include <new>

#include "types.h"
#include "gedx8musicdrv.h"
#include "gedx8_interface.h"

#pragma comment(lib, "Ole32.lib")

template<typename T>
struct Gedx8ObjectRegistry
{
	T** entries;       // +00
	u32 activeCount;   // +04
	u32 usedCount;     // +08
	u32 capacity;      // +0C
};

static_assert(sizeof(Gedx8ObjectRegistry<void>) == 0x10, "Gedx8ObjectRegistry must be 16 bytes");


struct Gedx8PerformanceOwner
{
	u32 reserved00;       // +00
	IUnknown* interface04; // +04
};

static_assert(sizeof(Gedx8PerformanceOwner) == 0x08, "Gedx8PerformanceOwner must be 8 bytes");


struct Gedx8DirectMusicOwner
{
	u32 reserved00;          // +00
	IUnknown* interface04;   // +04
	IUnknown* performance08; // +08, borrowed
};

static_assert(sizeof(Gedx8DirectMusicOwner) == 0x0C, "Gedx8DirectMusicOwner must be 12 bytes");


struct Gedx8ControllerOwner;

struct Gedx8ControllerState
{
	void* vtable00;                // +00
	Gedx8ControllerOwner* owner04; // +04
	s32 mode08;                    // +08
	void* object0C;                // +0C
	IUnknown* interface10;         // +10
	void* selected14;              // +14
};

static_assert(sizeof(Gedx8ControllerState) == 0x18, "Gedx8ControllerState must be 24 bytes");


struct Gedx8ControllerOwner
{
	Gedx8ControllerState* state00;       // +00
	u32 reserved04;                     // +04
	u32 reserved08;                     // +08
	IUnknown* performance0C;            // +0C, borrowed
	Gedx8DirectMusicOwner* directMusic10; // +10, borrowed
};

static_assert(sizeof(Gedx8ControllerOwner) == 0x14, "Gedx8ControllerOwner must be 20 bytes");


struct Gedx8DriverInstance
{
	u8 active;         // +00
	u8 reserved01[3];

	Gedx8ObjectRegistry<Gedx8Audiopath>* audiopaths;    // +04
	Gedx8ObjectRegistry<Gedx8LoadedObject>* objects;    // +08
	Gedx8PerformanceOwner* performance;                 // +0C
	Gedx8ControllerOwner* controller;                   // +10
	Gedx8DirectMusicOwner* directMusic;                 // +14
};

static_assert(sizeof(Gedx8DriverInstance) == 0x18, "Gedx8DriverInstance must be 24 bytes");


struct Gedx8LoadedObject
{
	u8 active;         // +00
	u8 reserved01[3];
	s32 kind;          // +04
	void* unknown08;   // +08
	void* payload;     // +0C
};

static_assert(sizeof(Gedx8LoadedObject) == 0x10, "Gedx8LoadedObject must be 16 bytes");


struct Gedx8Audiopath
{
	u8 active;                       // +000
	u8 reserved001[0x1BF];           // +001
	IUnknown* interface1C0;          // +1C0
	u32 unknown1C4;                  // +1C4
};

static_assert(sizeof(Gedx8Audiopath) == 0x1C8, "Gedx8Audiopath must be 456 bytes");


struct Gedx8InstanceRegistry
{
	Gedx8DriverInstance** entries; // +00
	u32 activeCount;               // +04
	u32 usedCount;                 // +08
	u32 capacity;                  // +0C
};

static_assert(sizeof(Gedx8InstanceRegistry) == 0x10, "Gedx8InstanceRegistry must be 16 bytes");


static Gedx8InstanceRegistry* g_instanceRegistry = nullptr;

static const GUID CLSID_1000C3F8 =
{
	0xD2AC2881,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID IID_1000C248 =
{
	0x679C4137,
	0xC62E,
	0x4147,
	{ 0xB2, 0xB4, 0x9D, 0x56, 0x9A, 0xCB, 0x25, 0x4C }
};

static const GUID CLSID_1000C3A8 =
{
	0xD2AC2890,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID IID_1000C278 =
{
	0xD2AC28BF,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

template<typename T>
static Gedx8ObjectRegistry<T>* CreateObjectRegistry()
{
	auto* registry = new (std::nothrow) Gedx8ObjectRegistry<T>{};

	if (registry == nullptr)
		return nullptr;

	registry->entries = new (std::nothrow) T * [1] {};

	if (registry->entries == nullptr)
	{
		delete registry;
		return nullptr;
	}

	registry->capacity = 1;
	return registry;
}


static Gedx8PerformanceOwner* CreatePerformanceOwner()
{
	auto* owner = new (std::nothrow) Gedx8PerformanceOwner{};

	if (owner == nullptr)
		return nullptr;

	// Original 10003CC0 ignoriert beide HRESULT-Werte.
	static_cast<void>(CoInitialize(nullptr));

	void* value = nullptr;

	static_cast<void>(CoCreateInstance(CLSID_1000C3F8, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, IID_1000C248, &value));

	owner->interface04 = static_cast<IUnknown*>(value);
	return owner;
}


static Gedx8DirectMusicOwner* CreateDirectMusicOwner(IUnknown* performance)
{
	auto* owner = new (std::nothrow) Gedx8DirectMusicOwner{};

	if (owner == nullptr)
		return nullptr;

	// Original 10002D90 ignoriert ebenfalls beide HRESULT-Werte.
	static_cast<void>(CoInitialize(nullptr));

	void* value = nullptr;

	static_cast<void>(CoCreateInstance(CLSID_1000C3A8, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, IID_1000C278, &value));

	owner->interface04 = static_cast<IUnknown*>(value);
	owner->performance08 = performance;

	return owner;
}


static Gedx8ControllerOwner* CreateControllerOwner(IUnknown* performance, Gedx8DirectMusicOwner* directMusic)
{
	auto* owner = new (std::nothrow) Gedx8ControllerOwner{};

	if (owner == nullptr)
		return nullptr;

	owner->state00 = new (std::nothrow) Gedx8ControllerState{};

	if (owner->state00 != nullptr)
	{
		// Der originale interne VTable-Pointer wird in unserer
		// Neuimplementierung später durch direkte C++-Methoden ersetzt.
		owner->state00->vtable00 = nullptr;
		owner->state00->owner04 = owner;
		owner->state00->mode08 = 3;
		owner->state00->object0C = nullptr;
		owner->state00->interface10 = nullptr;
		owner->state00->selected14 = nullptr;
	}

	owner->reserved04 = 0;
	owner->reserved08 = 0;
	owner->performance0C = performance;
	owner->directMusic10 = directMusic;

	return owner;
}


// -----------------------------------------------------------------------------
// Global instance registry
// Original: DAT_10010E20
// -----------------------------------------------------------------------------

static u32 CalculateNextCapacity(u32 currentCapacity)
{
	if (currentCapacity < 9)
		return currentCapacity + 4;

	if (currentCapacity < 65)
		return currentCapacity + 16;

	return currentCapacity + currentCapacity / 4;
}


static bool GrowInstanceRegistry(Gedx8InstanceRegistry* registry)
{
	if (registry == nullptr)
		return false;

	if (registry->usedCount > registry->capacity)
		return false;

	if (registry->usedCount != 0 && registry->entries == nullptr)
		return false;

	const u32 newCapacity = CalculateNextCapacity(registry->capacity);

	if (newCapacity <= registry->capacity)
		return false;

	Gedx8DriverInstance** newEntries = new (std::nothrow) Gedx8DriverInstance * [newCapacity];

	if (newEntries == nullptr)
		return false;

	for (u32 index = 0; index < newCapacity; ++index)
		newEntries[index] = nullptr;

	for (u32 index = 0; index < registry->usedCount; ++index)
		newEntries[index] = registry->entries[index];

	delete[] registry->entries;

	registry->entries = newEntries;
	registry->capacity = newCapacity;

	return true;
}

// -----------------------------------------------------------------------------
// Destructor for driver instances
// Original: 100010D0
// -----------------------------------------------------------------------------

static void ReleaseInterface(IUnknown*& value)
{
	if (value == nullptr)
		return;

	value->Release();
	value = nullptr;
}


static void DestroyPerformanceOwner(Gedx8PerformanceOwner*& owner)
{
	if (owner == nullptr)
		return;

	// Original: FUN_10003CF0.
	ReleaseInterface(owner->interface04);

	delete owner;
	owner = nullptr;
}


static void DestroyControllerOwner(Gedx8ControllerOwner*& owner)
{
	if (owner == nullptr)
		return;

	Gedx8ControllerState* const state = owner->state00;

	if (state != nullptr)
	{
		// Original: FUN_10003550 + FUN_10004966.
		//
		// FUN_10003550 setzt lediglich die interne VTable zurück.
		// Da diese Neuimplementierung keine Original-VTable verwendet,
		// ist nur die Speicherfreigabe erforderlich.
		if (state->object0C != nullptr)
		{
			::operator delete(state->object0C);
			state->object0C = nullptr;
		}

		// Original: Release auf dem Interface bei +10.
		ReleaseInterface(state->interface10);

		state->selected14 = nullptr;

		delete state;
		owner->state00 = nullptr;
	}

	// performance0C und directMusic10 sind nur geliehene Pointer.
	owner->performance0C = nullptr;
	owner->directMusic10 = nullptr;

	delete owner;
	owner = nullptr;
}


static void DestroyDirectMusicOwner(Gedx8DirectMusicOwner*& owner)
{
	if (owner == nullptr)
		return;

	// Original: FUN_10002DC0.
	ReleaseInterface(owner->interface04);

	// Geliehener Pointer, daher kein zweites Release.
	owner->performance08 = nullptr;

	delete owner;
	owner = nullptr;
}


static void DestroyAudiopath(Gedx8Audiopath* audiopath)
{
	if (audiopath == nullptr)
		return;

	// Original: FUN_100024B0.
	ReleaseInterface(audiopath->interface1C0);

	delete audiopath;
}


static void DestroyLoadedObject(Gedx8LoadedObject* object)
{
	if (object == nullptr)
		return;

	if (object->payload != nullptr)
	{
		switch (object->kind)
		{
		case 0:
		{
			// Original: FUN_10004170.
			//
			// Der Kind-0-Payload besitzt bei +10 einen zusätzlich
			// angelegten Speicherblock.
			auto* payloadBytes = static_cast<u8*>(object->payload);

			void*& allocation10 = *reinterpret_cast<void**>(payloadBytes + 0x10);

			if (allocation10 != nullptr)
			{
				::operator delete(allocation10);
				allocation10 = nullptr;
			}

			::operator delete(object->payload);
			object->payload = nullptr;
			break;
		}

		case 1:
		case 2:
			// Original: FUN_10003E80 ist leer.
			::operator delete(object->payload);
			object->payload = nullptr;
			break;

		default:
			// Das Original ruft für unbekannte Arten keinen
			// Payload-Destruktor und keine Payload-Freigabe auf.
			break;
		}
	}

	delete object;
}


static void DestroyAudiopathRegistry(Gedx8ObjectRegistry<Gedx8Audiopath>*& registry)
{
	if (registry == nullptr)
		return;

	for (u32 index = registry->usedCount; index > 0; --index)
	{
		Gedx8Audiopath*& audiopath = registry->entries[index - 1];

		if (audiopath == nullptr || audiopath->active == 0)
			continue;

		DestroyAudiopath(audiopath);
		audiopath = nullptr;
	}

	delete[] registry->entries;
	registry->entries = nullptr;

	delete registry;
	registry = nullptr;
}


static void DestroyLoadedObjectRegistry(Gedx8ObjectRegistry<Gedx8LoadedObject>*& registry)
{
	if (registry == nullptr)
		return;

	for (u32 index = registry->usedCount; index > 0; --index)
	{
		Gedx8LoadedObject*& object = registry->entries[index - 1];

		if (object == nullptr || object->active == 0)
			continue;

		DestroyLoadedObject(object);
		object = nullptr;
	}

	delete[] registry->entries;
	registry->entries = nullptr;

	delete registry;
	registry = nullptr;
}


static void DestroyDriverInstance(Gedx8DriverInstance* instance)
{
	if (instance == nullptr)
		return;

	// Reihenfolge aus 100010D0 und 10001630:
	//
	// +0C Performance
	// +10 Controller
	// +14 DirectMusic
	// +04 Audiopath-Registry
	// +08 LoadedObject-Registry
	// anschließend die Instanz selbst

	DestroyPerformanceOwner(instance->performance);
	DestroyControllerOwner(instance->controller);
	DestroyDirectMusicOwner(instance->directMusic);

	DestroyAudiopathRegistry(instance->audiopaths);
	DestroyLoadedObjectRegistry(instance->objects);

	instance->active = 0;

	delete instance;
}

// -----------------------------------------------------------------------------
// D3D8 availability probe
// Original: 100013D0
// -----------------------------------------------------------------------------

static u32 ProbeD3D8()
{
	HMODULE module = LoadLibraryA("D3D8.DLL");

	if (module == nullptr)
		return 0;

	FreeLibrary(module);
	return 0x800;
}


// -----------------------------------------------------------------------------
// Raw entry +00
// Original: 10001000
// -----------------------------------------------------------------------------

static u8 __stdcall Method10001000(s32 /*arg0*/, s32 /*arg1*/)
{
	return 1;
}


// -----------------------------------------------------------------------------
// +00
// -----------------------------------------------------------------------------

static void __stdcall Method10001090()
{
	// Das Original ignoriert den Rückgabewert.
	static_cast<void>(ProbeD3D8());

	g_instanceRegistry = nullptr;
	g_instanceRegistry = new (std::nothrow) Gedx8InstanceRegistry{};

	if (g_instanceRegistry == nullptr)
		return;

	// Die Wertinitialisierung durch {} setzt alle Felder auf null/0.
}


// -----------------------------------------------------------------------------
// +04
// -----------------------------------------------------------------------------

static void __stdcall Method100010D0()
{
	if (g_instanceRegistry == nullptr)
		return;

	for (u32 index = g_instanceRegistry->usedCount; index > 0; --index)
	{
		Gedx8DriverInstance*& instance = g_instanceRegistry->entries[index - 1];

		if (instance == nullptr || instance->active == 0)
			continue;

		DestroyDriverInstance(instance);
		instance = nullptr;
	}

	delete[] g_instanceRegistry->entries;
	g_instanceRegistry->entries = nullptr;

	delete g_instanceRegistry;
	g_instanceRegistry = nullptr;
}


// -----------------------------------------------------------------------------
// +08
// -----------------------------------------------------------------------------

static Gedx8DriverInstance* __stdcall Method100013F0()
{
	// Das Original setzt einen vorherigen Aufruf von 10001090 voraus.
	if (g_instanceRegistry == nullptr)
		return nullptr;

	auto* instance = new (std::nothrow) Gedx8DriverInstance{};

	if (instance == nullptr)
		return nullptr;

	instance->audiopaths = CreateObjectRegistry<Gedx8Audiopath>();

	instance->objects = CreateObjectRegistry<Gedx8LoadedObject>();

	if (instance->audiopaths == nullptr || instance->objects == nullptr)
	{
		DestroyDriverInstance(instance);
		return nullptr;
	}

	// Original: operator_new(8) -> FUN_10003CC0.
	instance->performance = CreatePerformanceOwner();

	if (instance->performance == nullptr)
	{
		DestroyDriverInstance(instance);
		return nullptr;
	}

	IUnknown* const performanceInterface = instance->performance->interface04;

	// Original: operator_new(0x0C) -> FUN_10002D90.
	instance->directMusic = CreateDirectMusicOwner(performanceInterface);

	if (instance->directMusic == nullptr)
	{
		DestroyDriverInstance(instance);
		return nullptr;
	}

	// Original: operator_new(0x14) -> FUN_10003810.
	instance->controller = CreateControllerOwner(performanceInterface, instance->directMusic);

	if (instance->controller == nullptr)
	{
		DestroyDriverInstance(instance);
		return nullptr;
	}

	u32 insertionIndex = g_instanceRegistry->usedCount;

	// Ersten freien oder inaktiven Slot suchen.
	for (u32 index = 0; index < g_instanceRegistry->usedCount; ++index)
	{
		Gedx8DriverInstance* const existing = g_instanceRegistry->entries[index];

		if (existing == nullptr || existing->active == 0)
		{
			insertionIndex = index;
			break;
		}
	}

	// Kein freier Slot: Array wie 100022C0 vergrößern.
	if (insertionIndex == g_instanceRegistry->usedCount)
	{
		if (g_instanceRegistry->usedCount == g_instanceRegistry->capacity)
		{
			if (!GrowInstanceRegistry(g_instanceRegistry))
			{
				DestroyDriverInstance(instance);
				return nullptr;
			}
		}

		++g_instanceRegistry->usedCount;
	}

	instance->active = 1;

	g_instanceRegistry->entries[insertionIndex] = instance;
	++g_instanceRegistry->activeCount;

	if (g_instanceRegistry->usedCount <= insertionIndex)
		g_instanceRegistry->usedCount = insertionIndex + 1;

	return instance;
}


// -----------------------------------------------------------------------------
// +0C
// -----------------------------------------------------------------------------

static u8 __cdecl Method10001630(Gedx8DriverInstance* instance)
{
	// Sicherheitsabweichung vom Original:
	// Das Original würde bei einer nicht initialisierten Registry abstürzen.
	if (g_instanceRegistry == nullptr)
		return 1;

	for (u32 index = g_instanceRegistry->usedCount; index > 0; --index)
	{
		Gedx8DriverInstance*& registeredInstance = g_instanceRegistry->entries[index - 1];

		if (registeredInstance != instance)
			continue;

		if (registeredInstance == nullptr || registeredInstance->active == 0)
		{
			continue;
		}

		DestroyDriverInstance(registeredInstance);
		registeredInstance = nullptr;
	}

	return 1;
}


// -----------------------------------------------------------------------------
// +10
// -----------------------------------------------------------------------------

static u8 __stdcall Method10001920(Gedx8DriverInstance* instance, const Gedx8SynthInitConfig* config)
{
	if (instance == nullptr)
		return 0;

	return 0;
}


static u8 __stdcall Method10001940(Gedx8DriverInstance* instance)
{
	return instance != nullptr;
}


static u8 __stdcall Method10001950(Gedx8DriverInstance* instance, s32 value)
{
	return instance != nullptr;
}


static u8 __stdcall Method10001970(Gedx8DriverInstance* instance, s32 value)
{
	return instance != nullptr;
}


// -----------------------------------------------------------------------------
// Loader
// -----------------------------------------------------------------------------

static u8 __stdcall Method10001990(Gedx8DriverInstance* instance, s32 kind, const Gedx8LoadDescriptor* descriptor, Gedx8LoadedObject** objectOut, const char* basePath)
{
	if (objectOut != nullptr)
		*objectOut = nullptr;

	return 0;
}


static u8 __stdcall Method10001AB0(Gedx8DriverInstance* instance, s32 value0, s32 value1)
{
	return 0;
}


static u8 __stdcall Method10001AD0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object)
{
	return 0;
}


static u8 __stdcall Method10001B50(Gedx8DriverInstance* instance)
{
	return 0;
}


static u8 __stdcall Method10001B60(Gedx8DriverInstance* instance)
{
	return 0;
}


// -----------------------------------------------------------------------------
// Audiopath
// -----------------------------------------------------------------------------

static u8 __stdcall Method10001B70(Gedx8DriverInstance* instance, void* config, Gedx8Audiopath** audiopathOut, Gedx8LoadedObject* object)
{
	if (audiopathOut != nullptr)
		*audiopathOut = nullptr;

	return 0;
}


static u8 __stdcall Method10001CF0(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 activeState)
{
	return 0;
}


static u8 __stdcall Method10001D10(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 volume, s32 fadeMilliseconds)
{
	return 0;
}


static u8 __stdcall Method10001D50(Gedx8DriverInstance* instance, s32 selector, s32 value, s32* storedValueOut)
{
	if (storedValueOut != nullptr)
		*storedValueOut = 0;

	return 0;
}


static u8 __stdcall Method10001D30(Gedx8DriverInstance* instance, s32* selectionOut)
{
	if (selectionOut != nullptr)
		*selectionOut = 0;

	return 0;
}


static u8 __stdcall Method10001D70(Gedx8DriverInstance* instance, s32 selector, s32 value, s32* storedValueOut)
{
	if (storedValueOut != nullptr)
		*storedValueOut = 0;

	return 0;
}


static u8 __stdcall Method10001D90(Gedx8DriverInstance* instance, u8 value)
{
	return 0;
}


static u8 __stdcall Method10001DA0(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath)
{
	return 0;
}


// -----------------------------------------------------------------------------
// Playback
// -----------------------------------------------------------------------------

static u8 __stdcall Method10001E30(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, Gedx8LoadedObject* object, s32 flags, s32 startTime, s32 repeatCount, s32 reserved)
{
	return 0;
}


static u8 __stdcall Method10001E70(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 stopMode)
{
	return 0;
}


static u8 __stdcall Method10001E90(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, u8* stateOut)
{
	if (stateOut != nullptr)
		*stateOut = 0;

	return 0;
}


// -----------------------------------------------------------------------------
// Composite helpers
// -----------------------------------------------------------------------------

static u8 __stdcall Method10001EB0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 mode, void* structure)
{
	return 0;
}


static u8 __stdcall Method10001EE0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 mode, void* structure)
{
	return 0;
}


static u8 __stdcall Method10001F10(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, u8* value0Out, u8* value1Out)
{
	if (value0Out != nullptr)
		*value0Out = 0;

	if (value1Out != nullptr)
		*value1Out = 0;

	return 0;
}


static u8 __stdcall Method10001F30(Gedx8DriverInstance* instance, Gedx8LoadedObject* object)
{
	return 0;
}


static u8 __stdcall Method10001FF0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object)
{
	return 0;
}


static u8 __stdcall Method10002010(Gedx8DriverInstance* instance, Gedx8LoadedObject* object)
{
	return 0;
}


static u8 __stdcall Method100020D0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object)
{
	return 0;
}


static u8 __stdcall Method100020F0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, const char* name)
{
	return 0;
}


static u8 __stdcall Method10002110(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, const char* name, s32 mode, void* payload)
{
	return 0;
}


static u8 __stdcall Method10002180(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, const char* name, s32 mode, void* payload)
{
	return 0;
}


static u8 __stdcall Method100021F0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object)
{
	return 0;
}


static u8 __stdcall Method100022B0(Gedx8DriverInstance* instance)
{
	return 0;
}


// -----------------------------------------------------------------------------
// Table
// -----------------------------------------------------------------------------

Gedx8MethodTable g_Gedx8MethodTable =
{
	&Method10001090, // +00
	&Method100010D0, // +04
	&Method100013F0, // +08
	&Method10001630, // +0C

	&Method10001920, // +10
	&Method10001940, // +14
	&Method10001950, // +18
	&Method10001970, // +1C

	&Method10001990, // +20
	&Method10001AB0, // +24
	&Method10001AD0, // +28
	&Method10001B50, // +2C
	&Method10001B60, // +30
	&Method10001B70, // +34
	&Method10001CF0, // +38
	&Method10001D10, // +3C

	&Method10001D50, // +40
	&Method10001D30, // +44
	&Method10001D70, // +48
	&Method10001D90, // +4C

	&Method10001DA0, // +50
	&Method10001E30, // +54
	&Method10001E70, // +58
	&Method10001E90, // +5C

	&Method10001EB0, // +60
	&Method10001EE0, // +64
	&Method10001F10, // +68
	&Method10001F30, // +6C

	&Method10001FF0, // +70

	&Method10001D90, // +74 alias

	&Method10002010, // +78
	&Method100020D0, // +7C
	&Method100020F0, // +80
	&Method10002110, // +84
	&Method10002180, // +88

	&Method10001D90, // +8C alias

	&Method100021F0, // +90
	&Method100022B0  // +94
};


Gedx8RawInterface g_Gedx8RawInterface =
{
	&Method10001000,
	&g_Gedx8MethodTable
};