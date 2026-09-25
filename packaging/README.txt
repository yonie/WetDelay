WET VST
=======

Audio effect plug-ins by Ronald Klarenbeek.

  Website:  https://wetvst.com
  Contact:  contact@wetvst.com
  Source:   https://github.com/yonie

The website has every WET plug-in and the latest versions.


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

  Both are signed and notarised by Apple, so macOS opens them without
  a warning. If it still refuses one (older releases were not signed),
  clear the download quarantine flag in Terminal:
    xattr -cr ~/Library/Audio/Plug-Ins/VST3/<Name>.vst3
    xattr -cr ~/Library/Audio/Plug-Ins/Components/<Name>.component

Linux
  Copy the .vst3 folder to:
    ~/.vst3/            (just you)
    /usr/lib/vst3/      (all users)

Then restart your DAW and rescan for plug-ins.
