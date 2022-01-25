//------------------------------------------------------------------------
// Copyright(c) 2021-2022 Olli Vanhoja.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace MyVst {
static const Steinberg::FUID kDemonProProcessorUID (0x95C62BD8, 0x75FF5DFB, 0x8D89C9C4, 0x1E898E0D);
static const Steinberg::FUID kDemonProControllerUID (0x35E48F5E, 0xA45B4A7B, 0x84810561, 0x9E1499C8);

#define DemonProVST3Category "Fx"
}
