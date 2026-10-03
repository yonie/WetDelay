//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------
// meterlevel.h - how a level meter's reading travels from the processor to the
// view. Canonical copy in the wet-toolkit hub (tools/meterlevel.h); each
// plug-in carries its own copy in source/, like monobus.h.
//
// A meter reading reaches the editor as a VST3 parameter, and the host clamps a
// parameter to 0..1. Sent as a linear peak, everything above 0 dBFS arrived as
// exactly 1.0, so a meter could never show an over and its red never lit for the
// right reason. The reading is sent as dB instead, mapped onto 0..1 over
// kFloorDb..kCeilDb, which leaves 12 dB of room above full scale for a float
// signal that goes past it.
//
// No hold of any kind: a reading falls back with the strip's normal decay
// (Ronald, 2026-10-03).
//
// Meter readings are not saved state, so changing this encoding needs no
// migration.
//------------------------------------------------------------------------

#pragma once

#include <cmath>

namespace Yonie {
namespace Meter {

constexpr double kFloorDb = -72.0;          // reads as 0: nothing lit
constexpr double kCeilDb  = 12.0;           // reads as 1

// Linear peak (1.0 = 0 dBFS) to the 0..1 value sent to the editor.
inline float encode(float linear)
{
    const double mag = std::fabs(static_cast<double>(linear));
    if (mag <= 0.0)
        return 0.f;
    const double db = 20.0 * std::log10(mag);
    const double n = (db - kFloorDb) / (kCeilDb - kFloorDb);
    return static_cast<float>(n < 0.0 ? 0.0 : n > 1.0 ? 1.0 : n);
}

// The 0..1 value back to dBFS, for the view.
inline double decodeDb(float normalized)
{
    return kFloorDb + static_cast<double>(normalized) * (kCeilDb - kFloorDb);
}

} // namespace Meter
} // namespace Yonie
