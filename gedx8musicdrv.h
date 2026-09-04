#pragma once

#include "types.h"

struct Gedx8SynthInitConfig
{
	s32 reserved00;
	s32 sampleRate;
	s32 config;
};

static_assert(sizeof(Gedx8SynthInitConfig) == 0x0C, "Gedx8SynthInitConfig must be 12 bytes");