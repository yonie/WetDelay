#!/bin/bash
set -e

echo "================================================"
echo " Merging platform binaries into universal VST3"
echo "================================================"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$SCRIPT_DIR"

# Create universal bundle structure
echo "Creating universal bundle structure..."
mkdir -p build/WetDelay.vst3/Contents/Resources
mkdir -p build/WetDelay.vst3/Contents/x86_64-win
mkdir -p build/WetDelay.vst3/Contents/x86_64-linux
mkdir -p build/WetDelay.vst3/Contents/arm64-macos

# Copy resources from Linux build (could be any platform - they're identical)
echo "Copying shared resources..."
if [ -d "artifacts/linux/Contents/Resources" ]; then
    cp -r artifacts/linux/Contents/Resources/* build/WetDelay.vst3/Contents/Resources/
elif [ -d "artifacts/windows/Contents/Resources" ]; then
    cp -r artifacts/windows/Contents/Resources/* build/WetDelay.vst3/Contents/Resources/
elif [ -d "artifacts/macos/Contents/Resources" ]; then
    cp -r artifacts/macos/Contents/Resources/* build/WetDelay.vst3/Contents/Resources/
fi

# Copy Windows binary
# Note: Windows binary in cross-platform bundle MUST have .vst3 extension (not .dll)
echo "Copying Windows binary..."
WINDOWS_FOUND=false
if [ -f "artifacts/windows/Contents/x86_64-win/WetDelay.vst3" ]; then
    cp artifacts/windows/Contents/x86_64-win/WetDelay.vst3 build/WetDelay.vst3/Contents/x86_64-win/
    echo "  Found at artifacts/windows/Contents/x86_64-win/WetDelay.vst3"
    WINDOWS_FOUND=true
elif [ -f "artifacts/windows/VST3/Release/WetDelay.vst3/Contents/x86_64-win/WetDelay.vst3" ]; then
    cp artifacts/windows/VST3/Release/WetDelay.vst3/Contents/x86_64-win/WetDelay.vst3 build/WetDelay.vst3/Contents/x86_64-win/
    echo "  Found at artifacts/windows/VST3/Release/WetDelay.vst3/Contents/x86_64-win/WetDelay.vst3"
    WINDOWS_FOUND=true
elif [ -f "artifacts/windows/Contents/x86_64-win/WetDelay.dll" ]; then
    # Legacy: rename .dll to .vst3 for cross-platform bundle
    cp artifacts/windows/Contents/x86_64-win/WetDelay.dll build/WetDelay.vst3/Contents/x86_64-win/WetDelay.vst3
    echo "  Found at artifacts/windows/Contents/x86_64-win/WetDelay.dll (renamed to .vst3)"
    WINDOWS_FOUND=true
elif [ -f "artifacts/windows/VST3/Release/WetDelay.vst3/Contents/x86_64-win/WetDelay.dll" ]; then
    # Legacy: rename .dll to .vst3 for cross-platform bundle
    cp artifacts/windows/VST3/Release/WetDelay.vst3/Contents/x86_64-win/WetDelay.dll build/WetDelay.vst3/Contents/x86_64-win/WetDelay.vst3
    echo "  Found at artifacts/windows/VST3/Release/WetDelay.vst3/Contents/x86_64-win/WetDelay.dll (renamed to .vst3)"
    WINDOWS_FOUND=true
fi
if [ "$WINDOWS_FOUND" = false ]; then
    echo "WARNING: Windows binary not found!"
    echo "Looking for Windows binary..."
    find artifacts/windows -name "*.dll" -o -name "WetDelay.vst3" 2>/dev/null | head -5 || echo "No Windows binary found"
fi

# Copy Linux binary
echo "Copying Linux binary..."
LINUX_FOUND=false
if [ -f "artifacts/linux/Contents/x86_64-linux/WetDelay.so" ]; then
    cp artifacts/linux/Contents/x86_64-linux/WetDelay.so build/WetDelay.vst3/Contents/x86_64-linux/
    echo "  Found at artifacts/linux/Contents/x86_64-linux/WetDelay.so"
    LINUX_FOUND=true
elif [ -f "artifacts/linux/VST3/Release/WetDelay.vst3/Contents/x86_64-linux/WetDelay.so" ]; then
    cp artifacts/linux/VST3/Release/WetDelay.vst3/Contents/x86_64-linux/WetDelay.so build/WetDelay.vst3/Contents/x86_64-linux/
    echo "  Found at artifacts/linux/VST3/Release/WetDelay.vst3/Contents/x86_64-linux/WetDelay.so"
    LINUX_FOUND=true
fi
if [ "$LINUX_FOUND" = false ]; then
    echo "WARNING: Linux binary not found!"
    find artifacts/linux -name "*.so" 2>/dev/null | head -5 || echo "No SO files found"
fi

# Copy macOS binary (may be in different location for Xcode builds)
echo "Copying macOS binary..."
MACOS_FOUND=false
if [ -f "artifacts/macos/Contents/arm64-macos/WetDelay" ]; then
    cp artifacts/macos/Contents/arm64-macos/WetDelay build/WetDelay.vst3/Contents/arm64-macos/
    echo "  Found at artifacts/macos/Contents/arm64-macos/WetDelay"
    MACOS_FOUND=true
elif [ -f "artifacts/macos/VST3/Release/WetDelay.vst3/Contents/MacOS/WetDelay" ]; then
    cp artifacts/macos/VST3/Release/WetDelay.vst3/Contents/MacOS/WetDelay build/WetDelay.vst3/Contents/arm64-macos/
    echo "  Found at artifacts/macos/VST3/Release/WetDelay.vst3/Contents/MacOS/WetDelay"
    MACOS_FOUND=true
elif [ -f "artifacts/macos/Contents/MacOS/WetDelay" ]; then
    cp artifacts/macos/Contents/MacOS/WetDelay build/WetDelay.vst3/Contents/arm64-macos/
    echo "  Found at artifacts/macos/Contents/MacOS/WetDelay"
    MACOS_FOUND=true
fi
if [ "$MACOS_FOUND" = false ]; then
    echo "WARNING: macOS binary not found!"
    find artifacts/macos -type f -name "WetDelay" 2>/dev/null | head -5 || echo "No WetDelay executable found"
fi

echo ""
echo "================================================"
echo " Universal VST3 bundle created!"
echo "================================================"
echo ""
echo "Bundle location: build/WetDelay.vst3"
echo ""
echo "Bundle structure:"
find build/WetDelay.vst3 -type f | head -20
echo ""
echo "Bundle size: $(du -sh build/WetDelay.vst3 | cut -f1)"
echo ""