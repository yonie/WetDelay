# WetDelay - agent instructions

Rules that hold for every change to this plug-in. Read them before touching the
processor, the parameters or the saved state.

## Saved settings are customer data

A plug-in's saved settings live inside the customer's DAW projects. Treat them
as customer data: if a control changes - more steps, a new range, a new taper,
a renamed or removed parameter - the new version must migrate the stored data so
every existing project opens exactly as it was saved. A mix that changes after
an update, with nothing to tell the customer, is the worst bug this plug-in can
have.

- The saved state must describe its own encoding (a version, and the step count
  each value was written on), so a later version knows how to read it.
- Before every release, load settings files saved by every earlier released
  version into the new build and check every value comes back unchanged.
- Save a set of settings files from each new release build and keep them, so the
  next version can be checked against this one. Once a release has shipped, that
  exact build is gone.

## Mono and stereo

All four bus layouts work: mono to mono, mono to stereo, stereo to stereo and
stereo to mono. A mono input is the same signal on both sides; a mono output is
the average of left and right. `source/monobus.h` does this - keep it, and keep
the Audio Unit's channel list in `resource/au-info.plist.in` listing all four.
Test every layout before a release, not just stereo.
