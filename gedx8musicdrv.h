#pragma once

#include "types.h"

struct Gedx8SynthInitConfig
{
	void* windowHandle; // +00, HWND in the original Win32 ABI
	u32 voiceCount;     // +04
	u32 sampleRate;     // +08
};

static_assert(sizeof(Gedx8SynthInitConfig) == 0x0C, "Gedx8SynthInitConfig must be 12 bytes");


struct Gedx8LoadDescriptor
{
	s32 objectKind;       // +00: 0 = Segment, 1 = Band, 2 = Script
	const char* fileName; // +04
	s32 metadata08;       // +08: observed 0x0E
	s32 metadata0C;       // +0C: observed 4
	s32 reserved10;       // +10: observed 0
};

static_assert(sizeof(Gedx8LoadDescriptor) == 0x14, "Gedx8LoadDescriptor must be 20 bytes");