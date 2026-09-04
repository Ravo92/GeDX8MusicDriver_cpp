#include <Windows.h>
#ifndef DIRECTSOUND_VERSION
#define DIRECTSOUND_VERSION 0x0800
#endif
#include <dsound.h>
#include <cstddef>
#include <new>

#include "types.h"
#include "gedx8musicdrv.h"
#include "gedx8_interface.h"

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Dxguid.lib")

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
	s32 masterVolume00;    // +00, cached GUID_PerfMasterVolume value
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
	s32 sourceMode;    // +08: 0/1/2 from FUN_10003890
	void* payload;     // +0C
};

static_assert(sizeof(Gedx8LoadedObject) == 0x10, "Gedx8LoadedObject must be 16 bytes");
static_assert(offsetof(Gedx8LoadedObject, kind) == 0x04, "Gedx8LoadedObject::kind offset mismatch");
static_assert(offsetof(Gedx8LoadedObject, sourceMode) == 0x08, "Gedx8LoadedObject::sourceMode offset mismatch");
static_assert(offsetof(Gedx8LoadedObject, payload) == 0x0C, "Gedx8LoadedObject::payload offset mismatch");


struct Gedx8SegmentPayload
{
	u32 reserved00;                       // +00
	IUnknown* segment04;                  // +04: IDirectMusicSegment8
	IUnknown* performance08;              // +08: borrowed
	Gedx8DirectMusicOwner* directMusic0C; // +0C: borrowed
	s32* resolved10;                      // +10: MUSIC_TIME table from FUN_10004670
	s32 length14;                         // +14: segment length
	u16 state18;                          // +18
	u8 state1A;                           // +1A
	u8 resolved1B;                        // +1B
};

static_assert(sizeof(Gedx8SegmentPayload) == 0x1C, "Gedx8SegmentPayload must be 28 bytes");
static_assert(offsetof(Gedx8SegmentPayload, segment04) == 0x04, "Gedx8SegmentPayload::segment04 offset mismatch");
static_assert(offsetof(Gedx8SegmentPayload, performance08) == 0x08, "Gedx8SegmentPayload::performance08 offset mismatch");
static_assert(offsetof(Gedx8SegmentPayload, directMusic0C) == 0x0C, "Gedx8SegmentPayload::directMusic0C offset mismatch");
static_assert(offsetof(Gedx8SegmentPayload, resolved10) == 0x10, "Gedx8SegmentPayload::resolved10 offset mismatch");
static_assert(offsetof(Gedx8SegmentPayload, length14) == 0x14, "Gedx8SegmentPayload::length14 offset mismatch");
static_assert(offsetof(Gedx8SegmentPayload, state18) == 0x18, "Gedx8SegmentPayload::state18 offset mismatch");
static_assert(offsetof(Gedx8SegmentPayload, state1A) == 0x1A, "Gedx8SegmentPayload::state1A offset mismatch");
static_assert(offsetof(Gedx8SegmentPayload, resolved1B) == 0x1B, "Gedx8SegmentPayload::resolved1B offset mismatch");


struct Gedx8TimeSignature
{
	s32 time00;
	u8 beatsPerMeasure04;
	u8 beat05;
	u16 gridsPerBeat06;
};

static_assert(sizeof(Gedx8TimeSignature) == 0x08, "DMUS_TIMESIGNATURE layout mismatch");


struct Gedx8TempoParam
{
	s32 time00;
	u32 padding04;
	u32 tempoLow08;
	u32 tempoHigh0C;
};

static_assert(sizeof(Gedx8TempoParam) == 0x10, "DMUS_TEMPO_PARAM layout mismatch");
static_assert(offsetof(Gedx8TempoParam, tempoLow08) == 0x08, "DMUS_TEMPO_PARAM tempo offset mismatch");


struct Gedx8SubChord
{
	u32 chordPattern00;
	u32 scalePattern04;
	u32 inversionPoints08;
	u32 levels0C;
	u8 chordRoot10;
	u8 scaleRoot11;
	u8 padding12[2];
};

static_assert(sizeof(Gedx8SubChord) == 0x14, "DMUS_SUBCHORD layout mismatch");


struct Gedx8ChordKey
{
	WCHAR name00[16];
	u16 measure20;
	u8 beat22;
	u8 subChordCount23;
	Gedx8SubChord subChords24[8];
	u32 scaleC4;
	u8 keyC8;
	u8 flagsC9;
	u8 paddingCA[2];
};

static_assert(sizeof(Gedx8ChordKey) == 0xCC, "DMUS_CHORD_KEY layout mismatch");
static_assert(offsetof(Gedx8ChordKey, measure20) == 0x20, "DMUS_CHORD_KEY measure offset mismatch");
static_assert(offsetof(Gedx8ChordKey, beat22) == 0x22, "DMUS_CHORD_KEY beat offset mismatch");
static_assert(offsetof(Gedx8ChordKey, subChordCount23) == 0x23, "DMUS_CHORD_KEY count offset mismatch");
static_assert(offsetof(Gedx8ChordKey, subChords24) + offsetof(Gedx8SubChord, chordRoot10) == 0x34, "DMUS_CHORD_KEY root offset mismatch");


struct Gedx8ChordControl
{
	u8 useCurrentTime00;
	u8 measure01;
	u8 beat02;
	u8 chordRoot03;
};

static_assert(sizeof(Gedx8ChordControl) == 0x04, "Gedx8ChordControl must be 4 bytes");


struct Gedx8Audiopath
{
	u8 active;                                  // +000
	u8 effectCount001;                          // +001
	u8 reserved002[0x02];                       // +002
	DSEFFECTDESC effectDescriptors004[9];       // +004
	u32 effectResults124[9];                    // +124
	IUnknown* parameterInterfaces148[13];       // +148
	u8 defaultParameters17C[0x34];              // +17C: original overlapping raw storage
	s32 pathType1B0;                 // +1B0
	u32 pchannelCount1B4;            // +1B4
	u8 activated1B8;                 // +1B8
	u8 reserved1B9[0x03];            // +1B9
	s32 volume1BC;                   // +1BC
	IUnknown* interface1C0;          // +1C0: IDirectMusicAudioPath8
	IUnknown* performance1C4;        // +1C4: borrowed
};

static_assert(sizeof(DSEFFECTDESC) == 0x20, "DSEFFECTDESC must be 32 bytes on Win32");
static_assert(sizeof(Gedx8Audiopath) == 0x1C8, "Gedx8Audiopath must be 456 bytes");
static_assert(offsetof(Gedx8Audiopath, effectDescriptors004) == 0x004, "Gedx8Audiopath::effectDescriptors004 offset mismatch");
static_assert(offsetof(Gedx8Audiopath, effectResults124) == 0x124, "Gedx8Audiopath::effectResults124 offset mismatch");
static_assert(offsetof(Gedx8Audiopath, parameterInterfaces148) == 0x148, "Gedx8Audiopath::parameterInterfaces148 offset mismatch");
static_assert(offsetof(Gedx8Audiopath, defaultParameters17C) == 0x17C, "Gedx8Audiopath::defaultParameters17C offset mismatch");
static_assert(offsetof(Gedx8Audiopath, pathType1B0) == 0x1B0, "Gedx8Audiopath::pathType1B0 offset mismatch");
static_assert(offsetof(Gedx8Audiopath, activated1B8) == 0x1B8, "Gedx8Audiopath::activated1B8 offset mismatch");
static_assert(offsetof(Gedx8Audiopath, volume1BC) == 0x1BC, "Gedx8Audiopath::volume1BC offset mismatch");
static_assert(offsetof(Gedx8Audiopath, interface1C0) == 0x1C0, "Gedx8Audiopath::interface1C0 offset mismatch");
static_assert(offsetof(Gedx8Audiopath, performance1C4) == 0x1C4, "Gedx8Audiopath::performance1C4 offset mismatch");


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

static const GUID CLSID_DirectMusicLoader_1000C398 =
{
	0xD2AC2892,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID IID_IDirectMusicLoader8_1000C258 =
{
	0x19E7C08C,
	0x0A44,
	0x4E6A,
	{ 0xA1, 0x16, 0x59, 0x5A, 0x7C, 0xD5, 0xDE, 0x8C }
};

static const GUID GUID_DirectMusicAllTypes_1000C324 =
{
	0xD2AC2893,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID CLSID_DirectMusicSegment_1000C218 =
{
	0xD2AC2882,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID IID_IDirectMusicSegment8_1000C238 =
{
	0xC6784488,
	0x41A3,
	0x418F,
	{ 0xAA, 0x15, 0xB3, 0x50, 0x93, 0xBA, 0x42, 0xD4 }
};

// DirectMusic performance parameters used by original FUN_10003D00,
// FUN_10003E20 and FUN_10003E50.
static const GUID GUID_PerfMasterTempo_1000C2B8 =
{
	0xD2AC28B0,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID GUID_PerfMasterGrooveLevel_1000C2C8 =
{
	0xD2AC28B2,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID GUID_PerfMasterVolume_1000C2D8 =
{
	0xD2AC28B1,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID GUID_PerfAutoDownload_1000C2E8 =
{
	0xFB09565B,
	0x3631,
	0x11D2,
	{ 0xBC, 0xB8, 0x00, 0xA0, 0xC9, 0x22, 0xE6, 0xEB }
};

static const GUID GUID_TempoParam_1000C2F8 =
{
	0xD2AC28A5,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID GUID_TimeSignature_1000C308 =
{
	0xD2AC28A4,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};

static const GUID GUID_ChordParam_1000C318 =
{
	0xD2AC289E,
	0xB39B,
	0x11D1,
	{ 0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xB1, 0xBD }
};


struct Gedx8AudioParams
{
	u32 size;
	BOOL initializeNow;
	u32 validData;
	u32 features;
	u32 voices;
	u32 sampleRate;
	GUID defaultSynth;
};

static_assert(sizeof(Gedx8AudioParams) == 0x28, "Gedx8AudioParams must be 40 bytes");


struct Gedx8ObjectDesc
{
	u32 size;
	u32 validData;
	GUID objectId;
	GUID classId;
	FILETIME date;
	u32 versionMS;
	u32 versionLS;
	WCHAR name[64];
	WCHAR category[64];
	WCHAR fileName[MAX_PATH];
	LONGLONG memoryLength;
	void* memoryData;
	void* stream;
};

static_assert(sizeof(Gedx8ObjectDesc) == 0x350, "Gedx8ObjectDesc must be 848 bytes");
static_assert(offsetof(Gedx8ObjectDesc, classId) == 0x18, "Gedx8ObjectDesc::classId offset mismatch");
static_assert(offsetof(Gedx8ObjectDesc, fileName) == 0x138, "Gedx8ObjectDesc::fileName offset mismatch");


template<typename T>
static T GetComMethod(IUnknown* interfaceValue, u32 byteOffset)
{
	void** const vtable = *reinterpret_cast<void***>(interfaceValue);
	return reinterpret_cast<T>(vtable[byteOffset / sizeof(void*)]);
}


using PerformanceGetGlobalParamFn = HRESULT(__stdcall*)(IUnknown* performance, const GUID& parameterType, void* value, u32 valueSize);
using PerformanceSetGlobalParamFn = HRESULT(__stdcall*)(IUnknown* performance, const GUID& parameterType, void* value, u32 valueSize);
using PerformanceCloseDownFn = HRESULT(__stdcall*)(IUnknown* performance);
using PerformanceInitAudioFn = HRESULT(__stdcall*)(IUnknown* performance, void** directMusicOut, void** directSoundOut, HWND windowHandle, u32 defaultPathType, u32 pchannelCount, u32 flags, Gedx8AudioParams* parameters);
using LoaderGetObjectFn = HRESULT(__stdcall*)(IUnknown* loader, Gedx8ObjectDesc* descriptor, const GUID& interfaceId, void** objectOut);
using LoaderSetSearchDirectoryFn = HRESULT(__stdcall*)(IUnknown* loader, const GUID& objectType, const WCHAR* path, BOOL clear);
using SegmentGetLengthFn = HRESULT(__stdcall*)(IUnknown* segment, s32* lengthOut);
using SegmentGetAudioPathConfigFn = HRESULT(__stdcall*)(IUnknown* segment, IUnknown** configOut);
using SegmentSetRepeatsFn = HRESULT(__stdcall*)(IUnknown* segment, s32 repeatCount);
using SegmentDownloadFn = HRESULT(__stdcall*)(IUnknown* segment, IUnknown* performance);
using SegmentGetParamFn = HRESULT(__stdcall*)(IUnknown* segment, const GUID& parameterType, u32 groupBits, u32 trackIndex, s32 time, s32* nextTimeOut, void* parameterData);
using SegmentSetParamFn = HRESULT(__stdcall*)(IUnknown* segment, const GUID& parameterType, u32 groupBits, u32 trackIndex, s32 time, void* parameterData);
using AudiopathActivateFn = HRESULT(__stdcall*)(IUnknown* audiopath, BOOL activate);
using AudiopathSetVolumeFn = HRESULT(__stdcall*)(IUnknown* audiopath, s32 volume, s32 duration);
using AudiopathGetObjectInPathFn = HRESULT(__stdcall*)(IUnknown* audiopath, s32 pchannel, u32 stage, u32 buffer, const GUID& objectGuid, u32 index, const GUID& interfaceId, void** objectOut);
using DirectSoundBufferSetFxFn = HRESULT(__stdcall*)(IUnknown* buffer, u32 effectCount, DSEFFECTDESC* effectDescriptors, u32* resultCodes);
using DirectSoundBufferGetObjectInPathFn = HRESULT(__stdcall*)(IUnknown* buffer, const GUID& objectGuid, u32 index, const GUID& interfaceId, void** objectOut);
using DirectSoundGetParameterFn = HRESULT(__stdcall*)(IUnknown* interfaceValue, void* parameterData);
using DirectSoundSetScalarFn = HRESULT(__stdcall*)(IUnknown* interfaceValue, u32 value);
using DirectSoundSetPositionFn = HRESULT(__stdcall*)(IUnknown* interfaceValue, u32 x, u32 y, u32 z, u32 applyMode);
using DirectSoundEffectParametersFn = HRESULT(__stdcall*)(IUnknown* effect, void* parameterData);
using PerformanceIsPlayingFn = HRESULT(__stdcall*)(IUnknown* performance, IUnknown* segment, IUnknown* segmentState);
using PerformancePlaySegmentExFn = HRESULT(__stdcall*)(IUnknown* performance, IUnknown* source, const WCHAR* segmentName, IUnknown* transition, u32 flags, u32 startTimeLow, u32 startTimeHigh, IUnknown** segmentStateOut, IUnknown* from, IUnknown* audiopath);
using PerformanceStopExFn = HRESULT(__stdcall*)(IUnknown* performance, IUnknown* objectToStop, u32 stopTimeLow, u32 stopTimeHigh, u32 flags);
using PerformanceSetDefaultAudioPathFn = HRESULT(__stdcall*)(IUnknown* performance, IUnknown* audiopath);
using PerformanceCreateAudioPathFn = HRESULT(__stdcall*)(IUnknown* performance, IUnknown* sourceConfig, BOOL activate, IUnknown** audiopathOut);
using PerformanceCreateStandardAudioPathFn = HRESULT(__stdcall*)(IUnknown* performance, u32 type, u32 pchannelCount, BOOL activate, IUnknown** audiopathOut);
using DirectMusicConfigureSegmentFn = HRESULT(__stdcall*)(IUnknown* directMusic, IUnknown* segment, u32 value00, u32 value04);
using PerformanceGetTimeFn = HRESULT(__stdcall*)(IUnknown* performance, LONGLONG* referenceTimeOut, s32* musicTimeOut);
using PerformanceInvalidateFn = HRESULT(__stdcall*)(IUnknown* performance, s32 musicTime, u32 flags);
using PerformanceRhythmToTimeFn = HRESULT(__stdcall*)(IUnknown* performance, u16 measure, u8 beat, u8 grid, s16 offset, const Gedx8TimeSignature* timeSignature, s32* musicTimeOut);

static u8 ResolveSegmentRhythm(Gedx8SegmentPayload* payload);


static Gedx8SegmentPayload* InitializeSegmentPayload(Gedx8SegmentPayload* payload, IUnknown* segment, IUnknown* performance, Gedx8DirectMusicOwner* directMusic)
{
	payload->state18 = 0;
	payload->state1A = 0;
	payload->length14 = 0;
	payload->resolved10 = nullptr;
	payload->resolved1B = 0;
	payload->segment04 = segment;

	const auto getLength = GetComMethod<SegmentGetLengthFn>(segment, 0x0C);
	static_cast<void>(getLength(segment, &payload->length14));

	payload->directMusic0C = directMusic;
	payload->performance08 = performance;
	payload->resolved1B = ResolveSegmentRhythm(payload);
	return payload;
}

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

	// Original 10003CC0 ignores both HRESULT values.
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

	// Original 10002D90 also ignores both HRESULT values.
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
		// The original internal vtable pointer will be replaced in our reimplementation later with direct C++ methods.
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


template<typename T>
static bool GrowObjectRegistry(Gedx8ObjectRegistry<T>* registry)
{
	if (registry == nullptr || registry->usedCount > registry->capacity)
		return false;

	if (registry->usedCount != 0 && registry->entries == nullptr)
		return false;

	const u32 newCapacity = CalculateNextCapacity(registry->capacity);

	if (newCapacity <= registry->capacity)
		return false;

	T** const newEntries = new (std::nothrow) T * [newCapacity] {};

	if (newEntries == nullptr)
		return false;

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
		// FUN_10003550 merely resets the internal vtable.
		// Since this reimplementation does not use the original vtable,
		// only freeing the memory is necessary.
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

	// performance0C and directMusic10 are only borrowed pointers.
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

	// Borrowed pointer, therefore no second Release.
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
			auto* payload = static_cast<Gedx8SegmentPayload*>(object->payload);

			if (payload->resolved10 != nullptr)
			{
				::operator delete(payload->resolved10);
				payload->resolved10 = nullptr;
			}

			ReleaseInterface(payload->segment04);
			payload->performance08 = nullptr;
			payload->directMusic0C = nullptr;
			delete payload;
			object->payload = nullptr;
			break;
		}

		case 1:
		case 2:
			// Original: FUN_10003E80 is empty.
			::operator delete(object->payload);
			object->payload = nullptr;
			break;

		default:
			// The original does not call a payload destructor or free the payload
			// for unknown kinds.
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

	// Order from 100010D0 and 10001630:
	//
	// +0C Performance
	// +10 Controller
	// +14 DirectMusic
	// +04 Audiopath registry
	// +08 LoadedObject registry
	// then the instance itself

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
	// The original ignores the return value.
	static_cast<void>(ProbeD3D8());

	g_instanceRegistry = nullptr;
	g_instanceRegistry = new (std::nothrow) Gedx8InstanceRegistry{};

	if (g_instanceRegistry == nullptr)
		return;

	// Value initialization by {} sets all fields to null/0.
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
	// The original assumes a prior call to 10001090.
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

	// Find first free or inactive slot.
	for (u32 index = 0; index < g_instanceRegistry->usedCount; ++index)
	{
		Gedx8DriverInstance* const existing = g_instanceRegistry->entries[index];

		if (existing == nullptr || existing->active == 0)
		{
			insertionIndex = index;
			break;
		}
	}

	// No free slot: grow the array like at 100022C0.
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
	// Safety deviation from the original:
	// The original would crash on an uninitialized registry.
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

static u8 InitializePerformance(Gedx8PerformanceOwner* owner, const Gedx8SynthInitConfig* config)
{
	Gedx8AudioParams parameters{};
	parameters.size = sizeof(parameters);
	parameters.initializeNow = TRUE;
	parameters.validData = 0x07;
	parameters.features = 0x3F;
	parameters.voices = config->voiceCount;
	parameters.sampleRate = config->sampleRate;

	IUnknown* const performance = owner->interface04;

	if (performance == nullptr)
		return 0;

	const auto initAudio = GetComMethod<PerformanceInitAudioFn>(performance, 0xB0);

	const HRESULT initResult = initAudio(performance, nullptr, nullptr, reinterpret_cast<HWND>(config->windowHandle), 0, 0, 0x3F, &parameters);

	if (FAILED(initResult))
		return 0;

	const auto setGlobalParam = GetComMethod<PerformanceSetGlobalParamFn>(performance, 0x88);

	float masterTempo = 1.0f;
	s8 masterGrooveLevel = 0;
	s32 autoDownload = 1;
	owner->masterVolume00 = 0;

	// The original ignores the HRESULT values of these four initializations.
	static_cast<void>(setGlobalParam(performance, GUID_PerfMasterTempo_1000C2B8, &masterTempo, sizeof(masterTempo)));
	static_cast<void>(setGlobalParam(performance, GUID_PerfMasterGrooveLevel_1000C2C8, &masterGrooveLevel, sizeof(masterGrooveLevel)));
	static_cast<void>(setGlobalParam(performance, GUID_PerfAutoDownload_1000C2E8, &autoDownload, sizeof(autoDownload)));
	static_cast<void>(setGlobalParam(performance, GUID_PerfMasterVolume_1000C2D8, &owner->masterVolume00, sizeof(owner->masterVolume00)));

	return 1;
}


static u8 ClosePerformance(Gedx8PerformanceOwner* owner)
{
	IUnknown* const performance = owner->interface04;

	if (performance == nullptr)
		return 0;

	const auto closeDown = GetComMethod<PerformanceCloseDownFn>(performance, 0x98);
	return SUCCEEDED(closeDown(performance)) ? 1 : 0;
}


static u8 SetPerformanceMasterVolume(Gedx8PerformanceOwner* owner, s32 value)
{
	IUnknown* const performance = owner->interface04;

	if (performance == nullptr)
		return 0;

	if (value != owner->masterVolume00)
	{
		owner->masterVolume00 = value;

		const auto setGlobalParam = GetComMethod<PerformanceSetGlobalParamFn>(performance, 0x88);

		// FUN_10003E20 updates the cache before the COM call and ignores its HRESULT.
		// Ein vorhandenes Performance-Interface bedeutet Erfolg.
		static_cast<void>(setGlobalParam(performance, GUID_PerfMasterVolume_1000C2D8, &value, sizeof(value)));
	}

	return 1;
}


static u8 GetPerformanceMasterVolume(Gedx8PerformanceOwner* owner, s32* valueOut)
{
	IUnknown* const performance = owner->interface04;

	if (performance == nullptr)
		return 0;

	const auto getGlobalParam = GetComMethod<PerformanceGetGlobalParamFn>(performance, 0x84);
	return SUCCEEDED(getGlobalParam(performance, GUID_PerfMasterVolume_1000C2D8, valueOut, sizeof(*valueOut))) ? 1 : 0;
}


static u8 __stdcall Method10001920(Gedx8DriverInstance* instance, const Gedx8SynthInitConfig* config)
{
	return InitializePerformance(instance->performance, config);
}


static u8 __stdcall Method10001940(Gedx8DriverInstance* instance)
{
	return ClosePerformance(instance->performance);
}


static u8 __stdcall Method10001950(Gedx8DriverInstance* instance, s32 value)
{
	return SetPerformanceMasterVolume(instance->performance, value);
}


static u8 __stdcall Method10001970(Gedx8DriverInstance* instance, s32* valueOut)
{
	return GetPerformanceMasterVolume(instance->performance, valueOut);
}


// -----------------------------------------------------------------------------
// Loader
// -----------------------------------------------------------------------------

static bool SelectExternalLoader(Gedx8ControllerOwner* controller, const char* basePath)
{
	Gedx8ControllerState* const state = controller->state00;
	state->mode08 = 2;

	if (state->interface10 == nullptr)
	{
		static_cast<void>(CoInitialize(nullptr));

		void* loader = nullptr;
		static_cast<void>(CoCreateInstance(CLSID_DirectMusicLoader_1000C398, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER, IID_IDirectMusicLoader8_1000C258, &loader));

		state->interface10 = static_cast<IUnknown*>(loader);
	}

	if (state->interface10 == nullptr)
		return false;

	state->selected14 = state->interface10;

	WCHAR widePath[MAX_PATH]{};
	static_cast<void>(MultiByteToWideChar(CP_ACP, 0, basePath, -1, widePath, MAX_PATH));

	const auto setSearchDirectory = GetComMethod<LoaderSetSearchDirectoryFn>(state->interface10, 0x14);
	return SUCCEEDED(setSearchDirectory(state->interface10, GUID_DirectMusicAllTypes_1000C324, widePath, FALSE));
}


static bool LoadObservedSegment(Gedx8ControllerOwner* controller, s32 loadMode, const Gedx8LoadDescriptor* descriptor, Gedx8LoadedObject** objectOut, const char* basePath)
{
	// Belegter Spielpfad aus 10003890:
	// loadMode != 1, basePath != nullptr -> Loader-Modus 2.
	if (loadMode == 1 || basePath == nullptr || descriptor->objectKind != 0)
		return false;

	if (!SelectExternalLoader(controller, basePath))
		return false;

	Gedx8ObjectDesc objectDescriptor{};
	objectDescriptor.size = sizeof(objectDescriptor);
	objectDescriptor.validData = 0x12; // DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME
	objectDescriptor.classId = CLSID_DirectMusicSegment_1000C218;

	static_cast<void>(MultiByteToWideChar(CP_ACP, 0, descriptor->fileName, -1, objectDescriptor.fileName, MAX_PATH));

	IUnknown* const loader = controller->state00->interface10;
	const auto getObject = GetComMethod<LoaderGetObjectFn>(loader, 0x0C);

	void* segmentValue = nullptr;
	const HRESULT loadResult = getObject(loader, &objectDescriptor, IID_IDirectMusicSegment8_1000C238, &segmentValue);

	if (FAILED(loadResult) || segmentValue == nullptr)
		return false;

	auto* payload = new (std::nothrow) Gedx8SegmentPayload{};

	if (payload == nullptr)
	{
		static_cast<IUnknown*>(segmentValue)->Release();
		return false;
	}

	static_cast<void>(InitializeSegmentPayload(payload, static_cast<IUnknown*>(segmentValue), controller->performance0C, controller->directMusic10));

	auto* loadedObject = new (std::nothrow) Gedx8LoadedObject{};

	if (loadedObject == nullptr)
	{
		if (payload->resolved10 != nullptr)
			::operator delete(payload->resolved10);

		ReleaseInterface(payload->segment04);
		delete payload;
		return false;
	}

	loadedObject->kind = 0;
	loadedObject->sourceMode = 2;
	loadedObject->payload = payload;
	*objectOut = loadedObject;
	return true;
}


static bool RegisterLoadedObject(Gedx8ObjectRegistry<Gedx8LoadedObject>* registry, Gedx8LoadedObject* object)
{
	u32 insertionIndex = registry->usedCount;

	for (u32 index = 0; index < registry->usedCount; ++index)
	{
		Gedx8LoadedObject* const existing = registry->entries[index];

		if (existing == nullptr || existing->active == 0)
		{
			insertionIndex = index;
			break;
		}
	}

	if (insertionIndex == registry->usedCount)
	{
		if (registry->usedCount == registry->capacity && !GrowObjectRegistry(registry))
			return false;

		++registry->usedCount;
	}

	object->active = 1;
	registry->entries[insertionIndex] = object;
	++registry->activeCount;

	if (registry->usedCount <= insertionIndex)
		registry->usedCount = insertionIndex + 1;

	return true;
}


static u8 __stdcall Method10001990(Gedx8DriverInstance* instance, s32 loadMode, const Gedx8LoadDescriptor* descriptor, Gedx8LoadedObject** objectOut, const char* basePath)
{
	if (instance == nullptr || instance->controller == nullptr || instance->objects == nullptr || descriptor == nullptr || objectOut == nullptr)
	{
		return 0;
	}

	if (!LoadObservedSegment(instance->controller, loadMode, descriptor, objectOut, basePath))
		return 0;

	Gedx8LoadedObject* const loadedObject = *objectOut;

	if (!RegisterLoadedObject(instance->objects, loadedObject))
	{
		DestroyLoadedObject(loadedObject);
		*objectOut = nullptr;
		return 0;
	}

	return 1;
}


static u8 __stdcall Method10001AB0(Gedx8DriverInstance* instance, s32 value0, s32 value1)
{
	if (instance == nullptr || instance->controller == nullptr || value0 == 0 || value1 == 0)
		return 0;

	// FUN_10003BC0: These two values later enable the internal load path (loadMode == 1) of FUN_10003890.
	instance->controller->reserved04 = static_cast<u32>(value0);
	instance->controller->reserved08 = static_cast<u32>(value1);
	return 1;
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

static u32 MapStandardAudiopathType(s32 type)
{
	switch (type)
	{
	case 0: return 6;
	case 1: return 7;
	case 2: return 8;
	case 3: return 1;
	default: return 0;
	}
}


static Gedx8Audiopath* CreateAudiopath(
	Gedx8PerformanceOwner* performanceOwner,
	const Gedx8AudiopathConfig* config,
	Gedx8SegmentPayload* segmentPayload)
{
	if (performanceOwner == nullptr || performanceOwner->interface04 == nullptr)
		return nullptr;

	auto* audiopath = new (std::nothrow) Gedx8Audiopath{};

	if (audiopath == nullptr)
		return nullptr;

	IUnknown* const performance = performanceOwner->interface04;
	audiopath->performance1C4 = performance;

	HRESULT result = E_FAIL;

	if (segmentPayload == nullptr)
	{
		if (config != nullptr)
		{
			audiopath->pathType1B0 = config->type;
			audiopath->pchannelCount1B4 = config->pchannelCount;

			const u32 standardType = MapStandardAudiopathType(config->type);
			const auto createStandard = GetComMethod<PerformanceCreateStandardAudioPathFn>(performance, 0xC4);
			result = createStandard(performance, standardType, config->pchannelCount, FALSE, &audiopath->interface1C0);
		}
	}
	else if (segmentPayload->segment04 != nullptr)
	{
		IUnknown* sourceConfig = nullptr;
		const auto getAudioPathConfig = GetComMethod<SegmentGetAudioPathConfigFn>(segmentPayload->segment04, 0x6C);
		result = getAudioPathConfig(segmentPayload->segment04, &sourceConfig);

		if (SUCCEEDED(result))
		{
			const auto createAudiopath = GetComMethod<PerformanceCreateAudioPathFn>(performance, 0xC0);
			result = createAudiopath(performance, sourceConfig, FALSE, &audiopath->interface1C0);
		}

		// The original does not free the pointer returned by GetAudioPathConfig in FUN_10002380.
		// Dieses Verhalten wird hier beibehalten.
	}

	if (FAILED(result) || audiopath->interface1C0 == nullptr)
	{
		DestroyAudiopath(audiopath);
		return nullptr;
	}

	return audiopath;
}


static bool RegisterAudiopath(Gedx8ObjectRegistry<Gedx8Audiopath>* registry, Gedx8Audiopath* audiopath)
{
	u32 insertionIndex = registry->usedCount;

	for (u32 index = 0; index < registry->usedCount; ++index)
	{
		Gedx8Audiopath* const existing = registry->entries[index];

		if (existing == nullptr || existing->active == 0)
		{
			insertionIndex = index;
			break;
		}
	}

	if (insertionIndex == registry->usedCount)
	{
		if (registry->usedCount == registry->capacity && !GrowObjectRegistry(registry))
			return false;

		++registry->usedCount;
	}

	audiopath->active = 1;
	registry->entries[insertionIndex] = audiopath;
	++registry->activeCount;

	if (registry->usedCount <= insertionIndex)
		registry->usedCount = insertionIndex + 1;

	return true;
}


static u8 __stdcall Method10001B70(Gedx8DriverInstance* instance, const Gedx8AudiopathConfig* config, Gedx8Audiopath** audiopathOut, Gedx8LoadedObject* object)
{
	if (instance == nullptr || instance->performance == nullptr || instance->audiopaths == nullptr || audiopathOut == nullptr)
		return 0;

	Gedx8SegmentPayload* segmentPayload = nullptr;

	if (object != nullptr)
	{
		if (object->kind != 0)
			return 0;

		segmentPayload = static_cast<Gedx8SegmentPayload*>(object->payload);
	}

	Gedx8Audiopath* const audiopath = CreateAudiopath(instance->performance, config, segmentPayload);

	if (audiopath == nullptr)
	{
		*audiopathOut = nullptr;
		return 0;
	}

	if (!RegisterAudiopath(instance->audiopaths, audiopath))
	{
		DestroyAudiopath(audiopath);
		*audiopathOut = nullptr;
		return 0;
	}

	*audiopathOut = audiopath;
	return 1;
}


static u8 __stdcall Method10001CF0(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 activeState)
{
	static_cast<void>(instance);

	if (audiopath == nullptr || audiopath->interface1C0 == nullptr)
		return 0;

	const u8 requestedState = static_cast<u8>(activeState);

	if (requestedState == audiopath->activated1B8)
		return 1;

	const auto activate = GetComMethod<AudiopathActivateFn>(audiopath->interface1C0, 0x10);

	if (FAILED(activate(audiopath->interface1C0, requestedState)))
		return 0;

	audiopath->activated1B8 = requestedState;
	return 1;
}


static u8 __stdcall Method10001D10(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 volume, s32 fadeMilliseconds)
{
	static_cast<void>(instance);

	if (audiopath == nullptr || audiopath->interface1C0 == nullptr)
		return 0;

	const auto setVolume = GetComMethod<AudiopathSetVolumeFn>(audiopath->interface1C0, 0x14);

	if (FAILED(setVolume(audiopath->interface1C0, volume, fadeMilliseconds)))
		return 0;

	audiopath->volume1BC = volume;
	return 1;
}


static const GUID GUID_AllObjects_1000C1C8 =
{
	0xAA114DE5,
	0xC262,
	0x4169,
	{ 0xA1, 0xC8, 0x23, 0xD6, 0x98, 0xCC, 0x73, 0xB5 }
};


static HRESULT GetAudiopathObject(Gedx8Audiopath* audiopath, const GUID& interfaceId, void** objectOut)
{
	IUnknown* const interfaceValue = audiopath->interface1C0;
	const auto getObjectInPath = GetComMethod<AudiopathGetObjectInPathFn>(interfaceValue, 0x0C);

	return getObjectInPath(interfaceValue, -5, 0x6000, 0, GUID_AllObjects_1000C1C8, 0, interfaceId, objectOut);
}


struct Gedx8EffectDefinition
{
	const GUID* classId;
	const GUID* interfaceId;
};


static const Gedx8EffectDefinition g_effectDefinitions[9] =
{
	{ &GUID_DSFX_STANDARD_CHORUS,         &IID_IDirectSoundFXChorus },
	{ &GUID_DSFX_STANDARD_COMPRESSOR,     &IID_IDirectSoundFXCompressor },
	{ &GUID_DSFX_STANDARD_DISTORTION,      &IID_IDirectSoundFXDistortion },
	{ &GUID_DSFX_STANDARD_ECHO,            &IID_IDirectSoundFXEcho },
	{ &GUID_DSFX_STANDARD_FLANGER,         &IID_IDirectSoundFXFlanger },
	{ &GUID_DSFX_STANDARD_GARGLE,          &IID_IDirectSoundFXGargle },
	{ &GUID_DSFX_STANDARD_I3DL2REVERB,     &IID_IDirectSoundFXI3DL2Reverb },
	{ &GUID_DSFX_STANDARD_PARAMEQ,         &IID_IDirectSoundFXParamEq },
	{ &GUID_DSFX_WAVES_REVERB,             &IID_IDirectSoundFXWavesReverb }
};


static HRESULT AddAudiopathEffect(
	Gedx8Audiopath* audiopath,
	const GUID& classId,
	const GUID& interfaceId,
	IUnknown** effectOut)
{
	const u8 effectIndex = audiopath->effectCount001;
	DSEFFECTDESC& descriptor = audiopath->effectDescriptors004[effectIndex];
	descriptor.dwSize = sizeof(DSEFFECTDESC);
	descriptor.guidDSFXClass = classId;

	// FUN_10002C90 initializes the local pointer with `this` before
	// FUN_10002C60 replaces it via IDirectMusicAudioPath8::GetObjectInPath.
	IUnknown* buffer = reinterpret_cast<IUnknown*>(audiopath);
	HRESULT result = GetAudiopathObject(audiopath, IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&buffer));

	if (FAILED(result))
		return result;

	const auto setFx = GetComMethod<DirectSoundBufferSetFxFn>(buffer, 0x54);
	result = setFx(buffer, static_cast<u32>(effectIndex) + 1, audiopath->effectDescriptors004, audiopath->effectResults124);

	if (FAILED(result))
		return result;

	++audiopath->effectCount001;

	const auto getObjectInPath = GetComMethod<DirectSoundBufferGetObjectInPathFn>(buffer, 0x5C);
	return getObjectInPath(buffer, classId, 0, interfaceId, reinterpret_cast<void**>(effectOut));
}


static u8 SetOrGetAudiopathParameter(Gedx8Audiopath* audiopath, s32 selector, void* parameterData, u8 setParameter)
{
	if (audiopath == nullptr || audiopath->interface1C0 == nullptr)
		return 0;

	if (selector < 0 || selector > 12)
		return 0;

	if (parameterData == nullptr && setParameter != 0)
	{
		parameterData = reinterpret_cast<u8*>(audiopath)
			+ 0x17C
			+ static_cast<u32>(selector) * sizeof(u32);
	}

	IUnknown*& parameterInterface = audiopath->parameterInterfaces148[selector];

	if (selector <= 3)
	{
		if (selector == 1 && audiopath->pathType1B0 != 2 && audiopath->pathType1B0 != 3)
			return 0;

		if (selector == 3 && audiopath->pathType1B0 != 0)
			return 0;

		if (parameterInterface == nullptr)
		{
			const GUID& interfaceId = selector == 3 ? IID_IDirectSound3DBuffer : IID_IDirectSoundBuffer8;

			if (FAILED(GetAudiopathObject(audiopath, interfaceId, reinterpret_cast<void**>(&parameterInterface))))
			{
				return 0;
			}
		}

		HRESULT result = E_FAIL;

		if (selector == 3)
		{
			if (setParameter != 0)
			{
				const u32* const position = static_cast<const u32*>(parameterData);
				const auto setPosition = GetComMethod<DirectSoundSetPositionFn>(parameterInterface, 0x4C);
				result = setPosition(parameterInterface, position[0], position[1], position[2], 0);
			}
			else
			{
				const auto getPosition = GetComMethod<DirectSoundGetParameterFn>(parameterInterface, 0x28);
				result = getPosition(parameterInterface, parameterData);
			}
		}
		else if (setParameter != 0)
		{
			static const u32 setOffsets[3] = { 0x3C, 0x40, 0x44 };
			const auto setValue = GetComMethod<DirectSoundSetScalarFn>(parameterInterface, setOffsets[selector]);
			result = setValue(parameterInterface, *static_cast<const u32*>(parameterData));
		}
		else
		{
			static const u32 getOffsets[3] = { 0x18, 0x1C, 0x20 };
			const auto getValue = GetComMethod<DirectSoundGetParameterFn>(parameterInterface, getOffsets[selector]);
			result = getValue(parameterInterface, parameterData);
		}

		return SUCCEEDED(result) ? 1 : 0;
	}

	if (parameterInterface == nullptr)
	{
		const Gedx8EffectDefinition& definition = g_effectDefinitions[selector - 4];

		if (FAILED(AddAudiopathEffect(audiopath, *definition.classId, *definition.interfaceId, &parameterInterface)))
		{
			return 0;
		}

		void* const defaultParameterData = reinterpret_cast<u8*>(audiopath) + 0x17C + static_cast<u32>(selector) * sizeof(u32);

		if (SetOrGetAudiopathParameter(audiopath, selector, defaultParameterData, 0) == 0)
			return 0;
	}

	const u32 methodOffset = setParameter != 0 ? 0x0C : 0x10;
	const auto accessParameters = GetComMethod<DirectSoundEffectParametersFn>(parameterInterface, methodOffset);
	return SUCCEEDED(accessParameters(parameterInterface, parameterData)) ? 1 : 0;
}


static u8 __stdcall Method10001D50(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 selector, void* parameterData)
{
	static_cast<void>(instance);
	return SetOrGetAudiopathParameter(audiopath, selector, parameterData, 1);
}


static u8 __stdcall Method10001D30(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32* volumeOut)
{
	static_cast<void>(instance);

	if (audiopath == nullptr || volumeOut == nullptr)
		return 0;

	*volumeOut = audiopath->volume1BC;
	return 1;
}


static u8 __stdcall Method10001D70(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, s32 selector, void* parameterData)
{
	static_cast<void>(instance);
	return SetOrGetAudiopathParameter(audiopath, selector, parameterData, 0);
}


static u8 __stdcall Method10001D90(Gedx8DriverInstance* instance, u32 unused0, u32 unused1)
{
	static_cast<void>(instance);
	static_cast<void>(unused0);
	static_cast<void>(unused1);
	return 0;
}


static u8 __stdcall Method10001DA0(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath)
{
	if (instance == nullptr || instance->audiopaths == nullptr)
		return 0;

	Gedx8ObjectRegistry<Gedx8Audiopath>* const registry = instance->audiopaths;

	for (u32 index = registry->usedCount; index > 0; --index)
	{
		const u32 entryIndex = index - 1;
		Gedx8Audiopath* const entry = registry->entries[entryIndex];

		if (entry != audiopath || entry == nullptr || entry->active == 0)
			continue;

		DestroyAudiopath(entry);
		registry->entries[entryIndex] = nullptr;

		// The original reads the entry that was just set to nullptr again
		// and therefore does not decrease activeCount. This behavior is intentional.
		return 1;
	}

	return 0;
}


// -----------------------------------------------------------------------------
// Playback
// -----------------------------------------------------------------------------

static u8 StartSegmentPlayback(Gedx8SegmentPayload* payload, Gedx8Audiopath* audiopath, u32 flags, const Gedx8StartInfo* startInfo, s32 repeatCount, u8 downloadBeforePlay)
{
	if (payload == nullptr || payload->segment04 == nullptr || payload->performance08 == nullptr ||
		audiopath == nullptr || audiopath->interface1C0 == nullptr)
	{
		return 0;
	}

	if (downloadBeforePlay == 1)
	{
		const auto download = GetComMethod<SegmentDownloadFn>(payload->segment04, 0x74);

		if (FAILED(download(payload->segment04, payload->performance08)))
			return 0;
	}

	const auto setRepeats = GetComMethod<SegmentSetRepeatsFn>(payload->segment04, 0x18);

	if (FAILED(setRepeats(payload->segment04, repeatCount)))
		return 0;

	if (startInfo != nullptr)
	{
		const auto setDefaultAudiopath = GetComMethod<PerformanceSetDefaultAudioPathFn>(payload->performance08, 0xC8);

		if (SUCCEEDED(setDefaultAudiopath(payload->performance08, audiopath->interface1C0)))
		{
			// FUN_10004190 ignores the return value of FUN_10002DD0.
			if (payload->directMusic0C != nullptr && payload->directMusic0C->interface04 != nullptr)
			{
				const auto configureSegment = GetComMethod<DirectMusicConfigureSegmentFn>(payload->directMusic0C->interface04, 0x2C);
				static_cast<void>(configureSegment(payload->directMusic0C->interface04, payload->segment04, startInfo->value00, startInfo->value04));
			}

			return 1;
		}
	}

	const auto playSegmentEx = GetComMethod<PerformancePlaySegmentExFn>(payload->performance08, 0xB4);
	const HRESULT result = playSegmentEx(payload->performance08, payload->segment04, nullptr, nullptr, flags, 0, 0, nullptr, nullptr, audiopath->interface1C0);

	return SUCCEEDED(result) ? 1 : 0;
}


static u8 __stdcall Method10001E30(Gedx8DriverInstance* instance, Gedx8Audiopath* audiopath, Gedx8LoadedObject* object, u32 flags, const Gedx8StartInfo* startInfo, s32 repeatCount, u8 downloadBeforePlay)
{
	static_cast<void>(instance);

	if (object == nullptr || object->kind != 0)
		return 0;

	return StartSegmentPlayback(static_cast<Gedx8SegmentPayload*>(object->payload), audiopath, flags, startInfo, repeatCount, downloadBeforePlay);
}


static u8 StopSegmentPlayback(Gedx8SegmentPayload* payload, u32 flags)
{
	if (payload == nullptr || payload->segment04 == nullptr || payload->performance08 == nullptr)
		return 0;

	const auto stopEx = GetComMethod<PerformanceStopExFn>(payload->performance08, 0xB8);
	return SUCCEEDED(stopEx(payload->performance08, payload->segment04, 0, 0, flags)) ? 1 : 0;
}


static u8 __stdcall Method10001E70(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 stopMode)
{
	static_cast<void>(instance);

	if (object == nullptr || object->kind != 0)
		return 0;

	return StopSegmentPlayback(static_cast<Gedx8SegmentPayload*>(object->payload), static_cast<u32>(stopMode));
}


static u8 GetSegmentPlaybackState(Gedx8SegmentPayload* payload, u8* stateOut)
{
	if (stateOut == nullptr)
		return 0;

	*stateOut = 0;

	if (payload == nullptr || payload->segment04 == nullptr || payload->performance08 == nullptr)
		return 0;

	const auto isPlaying = GetComMethod<PerformanceIsPlayingFn>(payload->performance08, 0x38);
	const HRESULT result = isPlaying(payload->performance08, payload->segment04, nullptr);

	if (result == 0)
		*stateOut = 1;

	return SUCCEEDED(result) ? 1 : 0;
}


static u8 __stdcall Method10001E90(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, u8* stateOut)
{
	static_cast<void>(instance);

	if (object == nullptr || object->kind != 0)
		return 0;

	return GetSegmentPlaybackState(static_cast<Gedx8SegmentPayload*>(object->payload), stateOut);
}


// -----------------------------------------------------------------------------
// Composite helpers
// -----------------------------------------------------------------------------

static u8 ResolveSegmentRhythm(Gedx8SegmentPayload* payload)
{
	Gedx8ChordKey finalChord{};
	const auto getParam = GetComMethod<SegmentGetParamFn>(payload->segment04, 0x48);

	// FUN_10004670 ignores this first HRESULT. If the query fails, the
	// zero-filled chord keeps measure20 at zero and the function fails below.
	static_cast<void>(getParam(payload->segment04, GUID_ChordParam_1000C318, 0xFFFFFFFFu, 0x80000000u, payload->length14, nullptr, &finalChord));

	Gedx8TimeSignature timeSignature{};
	const HRESULT signatureResult = getParam(payload->segment04, GUID_TimeSignature_1000C308, 0xFFFFFFFFu, 0x80000000u, 0, nullptr, &timeSignature);

	if (FAILED(signatureResult) || finalChord.measure20 == 0)
		return 0;

	payload->state1A = timeSignature.beatsPerMeasure04;
	payload->state18 = finalChord.measure20;

	const u32 entryCount = static_cast<u32>(payload->state1A) * payload->state18;
	payload->resolved10 = static_cast<s32*>(::operator new(entryCount * sizeof(s32)));

	const auto rhythmToTime = GetComMethod<PerformanceRhythmToTimeFn>(payload->performance08, 0xAC);
	HRESULT result = signatureResult;
	u8 measure = 0;

	do
	{
		u8 beat = 0;

		while (beat < payload->state1A)
		{
			const u8 index = static_cast<u8>(payload->state1A * measure + beat);
			result = rhythmToTime(payload->performance08, measure, beat, 0, 0, &timeSignature, &payload->resolved10[index]);
			++beat;
		}

		++measure;
	} while (measure < payload->state18);

	return SUCCEEDED(result) ? 1 : 0;
}


static u8 SetSegmentComposite(Gedx8SegmentPayload* payload, s32 mode, void** structureAddress)
{
	if (payload->segment04 == nullptr)
		return 0;

	const auto setParam = GetComMethod<SegmentSetParamFn>(payload->segment04, 0x4C);

	if (mode == 0)
	{
		if (structureAddress == nullptr)
			return 0;

		const auto* const tempoWords = static_cast<const u32*>(*structureAddress);
		s32 currentTime = 0;
		const auto getTime = GetComMethod<PerformanceGetTimeFn>(payload->performance08, 0x3C);

		if (FAILED(getTime(payload->performance08, nullptr, &currentTime)))
			return 0;

		Gedx8TempoParam tempo{};
		tempo.time00 = currentTime;
		tempo.tempoLow08 = tempoWords[0];
		tempo.tempoHigh0C = tempoWords[1];

		if (FAILED(setParam(payload->segment04, GUID_TempoParam_1000C2F8, 0xFFFFFFFFu, 0x80000000u, 0, &tempo)))
		{
			return 0;
		}
	}
	else if (mode == 1)
	{
		if (structureAddress == nullptr || payload->resolved1B == 0)
			return 0;

		auto* const control = static_cast<Gedx8ChordControl*>(*structureAddress);

		if (control->measure01 > payload->state18 || control->beat02 > payload->state1A)
			return 0;

		const u8 index = static_cast<u8>(payload->state1A * control->measure01 + control->beat02);
		const s32 time = payload->resolved10[index];
		Gedx8ChordKey chord{};
		const auto getParam = GetComMethod<SegmentGetParamFn>(payload->segment04, 0x48);

		if (FAILED(getParam(payload->segment04, GUID_ChordParam_1000C318, 0xFFFFFFFFu, 0x80000000u, time, nullptr, &chord)))
		{
			return 0;
		}

		for (u8 subChord = 0; subChord < chord.subChordCount23; ++subChord)
			chord.subChords24[subChord].chordRoot10 = control->chordRoot03;

		if (FAILED(setParam(payload->segment04, GUID_ChordParam_1000C318, 0xFFFFFFFFu, 0x80000000u, time, &chord)))
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	const auto invalidate = GetComMethod<PerformanceInvalidateFn>(payload->performance08, 0x78);
	return SUCCEEDED(invalidate(payload->performance08, 0, 0x2000u)) ? 1 : 0;
}


static u8 GetSegmentComposite(Gedx8SegmentPayload* payload, s32 mode, void** structureAddress)
{
	if (payload->segment04 == nullptr || structureAddress == nullptr)
		return 0;

	void* const structure = *structureAddress;

	if (structure == nullptr)
		return 0;

	const auto getParam = GetComMethod<SegmentGetParamFn>(payload->segment04, 0x48);

	if (mode == 0)
	{
		Gedx8TempoParam tempo{};
		s32 currentTime = 0;
		const auto getTime = GetComMethod<PerformanceGetTimeFn>(payload->performance08, 0x3C);

		if (FAILED(getTime(payload->performance08, nullptr, &currentTime)))
			return 0;

		const HRESULT result = getParam(payload->segment04, GUID_TempoParam_1000C2F8, 0xFFFFFFFFu, 0x80000000u, 0, nullptr, &tempo);

		auto* const tempoWords = static_cast<u32*>(structure);
		tempoWords[0] = tempo.tempoLow08;
		tempoWords[1] = tempo.tempoHigh0C;
		return SUCCEEDED(result) ? 1 : 0;
	}

	if (mode != 1)
		return 0;

	auto* const control = static_cast<Gedx8ChordControl*>(structure);
	Gedx8ChordKey chord{};

	if (control->useCurrentTime00 != 0)
	{
		s32 currentTime = 0;
		const auto getTime = GetComMethod<PerformanceGetTimeFn>(payload->performance08, 0x3C);

		if (FAILED(getTime(payload->performance08, nullptr, &currentTime)))
			return 0;

		if (FAILED(getParam(payload->segment04, GUID_ChordParam_1000C318, 0xFFFFFFFFu, 0x80000000u, currentTime, nullptr, &chord)))
		{
			return 0;
		}

		control->measure01 = static_cast<u8>(chord.measure20);
		control->beat02 = chord.beat22;
		control->chordRoot03 = chord.subChords24[0].chordRoot10;
		return 1;
	}

	if (control->measure01 > payload->state18 || control->beat02 > payload->state1A || payload->resolved1B == 0)
	{
		return 0;
	}

	const u8 index = static_cast<u8>(payload->state1A * control->measure01 + control->beat02);

	if (FAILED(getParam(payload->segment04, GUID_ChordParam_1000C318, 0xFFFFFFFFu, 0x80000000u, payload->resolved10[index], nullptr, &chord)))
	{
		return 0;
	}

	control->chordRoot03 = chord.subChords24[0].chordRoot10;
	return 1;
}


static u8 GetSegmentRhythmDimensions(Gedx8SegmentPayload* payload, u8* valuesOut)
{
	if (valuesOut == nullptr || payload->resolved1B == 0)
		return 0;

	valuesOut[0] = static_cast<u8>(payload->state18);
	valuesOut[1] = payload->state1A;
	return 1;
}


static u8 __stdcall Method10001EB0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 mode, void* structure)
{
	static_cast<void>(instance);

	if (object->kind != 0)
		return 0;

	return SetSegmentComposite(static_cast<Gedx8SegmentPayload*>(object->payload), mode, &structure);
}


static u8 __stdcall Method10001EE0(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, s32 mode, void* structure)
{
	static_cast<void>(instance);

	if (object->kind != 0)
		return 0;

	return GetSegmentComposite(static_cast<Gedx8SegmentPayload*>(object->payload), mode, &structure);
}


static u8 __stdcall Method10001F10(Gedx8DriverInstance* instance, Gedx8LoadedObject* object, u8* valuesOut)
{
	static_cast<void>(instance);

	if (object->kind != 0)
		return 0;

	return GetSegmentRhythmDimensions(static_cast<Gedx8SegmentPayload*>(object->payload), valuesOut);
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