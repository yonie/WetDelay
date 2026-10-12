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
// THE LINE'S LEVEL METER (Ronald, 2026-10-04). A meter of n blocks: the first
// n-1 split kLowDb..kHighDb (-48 to 0 dBFS) into equal bands, and a band's block
// lights once the reading reaches the band's bottom. The last block is one red
// lamp that lights only above 0 dBFS. A band is yellow when its top lies above
// -12 dBFS, green otherwise. The block count is the ONLY thing a panel chooses;
// a view calls litSegments() and segmentZone() and carries no ladder of its own.
// tools/linemeter.py is the Python twin - change both together.
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

constexpr double kLowDb  = -48.0;           // the bottom of the bottom band
constexpr double kHighDb = 0.0;             // the top of the top band; above it, the red lamp

// The width of one band in a meter of n blocks.
inline double bandDb(int n)
{
    return n > 1 ? (kHighDb - kLowDb) / (n - 1) : (kHighDb - kLowDb);
}

// The level at which block i of n lights, counted from the bottom. The last
// block, the red lamp, lights only ABOVE this figure (0 dBFS).
inline double segmentDb(int i, int n)
{
    return i >= n - 1 ? kHighDb : kLowDb + i * bandDb(n);
}

// How many of n blocks a reading lights.
inline int litSegments(float normalized, int n)
{
    if (normalized <= 1e-6f || n < 1)
        return 0;
    const double db = decodeDb(normalized);
    if (db > kHighDb)
        return n;
    int lit = 0;
    for (int i = 0; i < n - 1; ++i)
        if (db >= segmentDb(i, n) - 1e-9)
            lit = i + 1;
    return lit;
}

enum Zone { kGreen = 0, kYellow = 1, kRed = 2 };

inline int segmentZone(int i, int n)
{
    if (i >= n - 1)
        return kRed;
    const double top = kLowDb + (i + 1) * bandDb(n);
    return top > -12.0 + 1e-9 ? kYellow : kGreen;
}

} // namespace Meter
} // namespace Yonie
