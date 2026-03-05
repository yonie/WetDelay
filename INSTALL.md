# WetDelay Installation Guide

## Quick Start

### Windows

1. Extract the ZIP file
2. Copy `WetDelay.vst3` to your VST3 folder:
   - **User**: `C:\Users\[Username]\Documents\VST3\`
   - **System**: `C:\Program Files\Common Files\VST3\`
3. Restart your DAW and rescan plugins

### Linux

1. Extract the ZIP file
2. Copy `WetDelay.vst3` to your VST3 folder:
   - **User**: `~/.vst3/`
   - **System**: `/usr/lib/vst3/`
3. Restart your DAW and rescan plugins

### macOS

1. Extract the ZIP file
2. Copy `WetDelay.vst3` to your VST3 folder:
   - **User** (recommended): `~/Library/Audio/Plug-Ins/VST3/`
   - **System**: `/Library/Audio/Plug-Ins/VST3/`
3. **Remove quarantine attribute** (see below)
4. Restart your DAW and rescan plugins

---

## macOS: Why Is This Blocked?

When you try to load the plugin, you may see:

> "WetDelay.vst3" cannot be opened because the developer cannot be verified.

**This is normal for free, open-source software.**

Apple requires developers to:
- Pay **$99/year** for a developer certificate
- Notarize each build with Apple

As an independent developer releasing free software, I don't have the budget for this. This is a common issue with free audio plugins on macOS - you'll see this with many open-source VSTs.

**The plugin is safe** - the complete source code is available on GitHub for inspection.

---

## macOS: How to Fix

Open Terminal and run:

```bash
sudo xattr -rd com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/WetDelay.vst3
```

**For system-wide installation:**
```bash
sudo xattr -rd com.apple.quarantine /Library/Audio/Plug-Ins/VST3/WetDelay.vst3
```

**What this does:**
- Removes the quarantine attribute macOS adds to downloaded files
- Requires administrator password (sudo)
- Run once after installation
- Restart your DAW after running

**Optional: Ad-Hoc Code Signing**

If the above doesn't work, sign the plugin locally:

```bash
codesign --force --deep --sign - ~/Library/Audio/Plug-Ins/VST3/WetDelay.vst3
```

---

## Still Having Issues?

**Plugin not appearing in DAW:**
- You forgot to remove the quarantine attribute (macOS)
- Restart your DAW after running the `xattr` command
- Check the VST3 path is correct
- Verify `WetDelay.vst3` folder exists in the path

**Still getting "cannot be verified":**
- Try the ad-hoc code signing method above
- Right-click the plugin → "Open" → "Open"
- Check macOS System Preferences → Privacy & Security

**Plugin crashes:**
- macOS 10.13+ (Intel) or macOS 11.0+ (Apple Silicon) required
- Windows 10/11 (64-bit) required
- Linux x86_64 required
- Report issues at: https://github.com/yonie/wetdelay/issues

---

## Support This Project

If you find this plugin helpful, consider supporting development:

☕ [Buy Me a Coffee](https://buymeacoffee.com/yonie)

Your support helps fund:
- Apple Developer Program enrollment (proper macOS signing)
- Future plugin development
- Continued open-source audio software

---

## System Requirements

- **Windows**: Windows 10/11 (64-bit)
- **Linux**: x86_64, GCC/Clang with C++17 support
- **macOS**: 
  - Intel: macOS 10.13 (High Sierra) or higher
  - Apple Silicon: macOS 11.0 (Big Sur) or higher

## What's Included

Your download contains a **universal VST3 bundle** with:
- **Windows** x64 binary
- **Linux** x86_64 binary
- **macOS** universal binary (Intel x86_64 + Apple Silicon arm64)

All platforms in a single plugin file!

---

## License

MIT License - Free for personal and commercial use.

Source code: https://github.com/yonie/wetdelay

---

**Built with ❤️ by an independent developer**

Thank you for using WetDelay! 🎛️