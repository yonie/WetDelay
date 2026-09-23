WET VST
=======

Audio effect plug-ins by Ronald Klarenbeek.

  Website:  https://wetvst.com
  Contact:  contact@wetvst.com
  Source:   https://github.com/yonie

This file comes in every WET download, so a zip found on a disk years
from now still says what it is and where it goes.


THE PLUG-INS
------------

  WetDelay       Stereo delay with the grain of a 1980s digital rack
                 unit. Six fixed delay times, 20 to 400 ms.
  WetReverb      Reverb in five modes: Room, Plate, Hall, Cathedral,
                 Cosmos.
  WetEQ          Four-band British console equaliser with high-pass
                 and low-pass filters. Stepped knobs.
  WetCompressor  FET compressor after a 1960s rack limiter. Input,
                 output and three attack/release speeds.
  WetChorus      Bucket-brigade stereo chorus from a 1970s solid-state
                 combo amp. Speed, a vibrato-to-chorus mode knob and
                 a depth button.
  WetWeld        Mix-bus chain in one panel: input gain, high-pass,
                 colour, low and high shelves, compression, widening
                 and output gain.

wetvst.com has the current list and the latest versions.


WHAT IS IN THIS ZIP
-------------------

  <Name>.vst3        The VST3 plug-in. One bundle runs on Windows,
                     macOS (Intel and Apple Silicon) and Linux.
  <Name>.component   The Audio Unit, for Logic and GarageBand on macOS.
                     Only in downloads that include it.
  README.txt         This file.


INSTALLATION
------------

Windows
  Copy the .vst3 folder to:
    C:\Program Files\Common Files\VST3\

macOS
  Copy the .vst3 to:
    ~/Library/Audio/Plug-Ins/VST3/
  Copy the .component (Logic, GarageBand) to:
    ~/Library/Audio/Plug-Ins/Components/

  Current WET releases are signed and notarised by Apple and open
  normally. Older downloads were not, and macOS refuses to load them
  until the quarantine flag is cleared in Terminal, for example:
    xattr -cr ~/Library/Audio/Plug-Ins/VST3/WetEQ.vst3
    xattr -cr ~/Library/Audio/Plug-Ins/Components/WetEQ.component

Linux
  Copy the .vst3 folder to:
    ~/.vst3/            (just you)
    /usr/lib/vst3/      (all users)

Then restart your DAW and rescan for plug-ins.

The WET effects in the delay and reverb family are 100% wet with no
mix knob. Put them on a send/return (aux) track, not on the channel
insert.


RESIZING
--------

Right-click the panel and choose UI Zoom: 75%, 100% or 125%.
