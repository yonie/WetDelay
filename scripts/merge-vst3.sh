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
cp -r artifacts/linux/Contents/Resources/* build/WetDelay.vst3/Contents/Resources/

# Copy Windows binary
echo "Copying Windows binary..."
if [ -f "artifacts/windows/Contents/x86_64-win/WetDelay.dll" ]; then
    cp artifacts/windows/Contents/x86_64-win/WetDelay.dll build/WetDelay.vst3/Contents/x86_64-win/
else
    echo "WARNING: Windows binary not found at artifacts/windows/Contents/x86_64-win/WetDelay.dll"
    ls -la artifacts/windows/Contents/ 2>/dev/null || echo "Windows artifact directory structure:"
    find artifacts/windows -name "*.dll" 2>/dev/null || echo "No DLL files found"
fi

# Copy Linux binary
echo "Copying Linux binary..."
if [ -f "artifacts/linux/Contents/x86_64-linux/WetDelay.so" ]; then
    cp artifacts/linux/Contents/x86_64-linux/WetDelay.so build/WetDelay.vst3/Contents/x86_64-linux/
else
    echo "WARNING: Linux binary not found at artifacts/linux/Contents/x86_64-linux/WetDelay.so"
    ls -la artifacts/linux/Contents/ 2>/dev/null || echo "Linux artifact directory structure:"
    find artifacts/linux -name "*.so" 2>/dev/null || echo "No SO files found"
fi

# Copy macOS binary (may be in different location for Xcode builds)
echo "Copying macOS binary..."
if [ -f "artifacts/macos/Contents/arm64-macos/WetDelay" ]; then
    cp artifacts/macos/Contents/arm64-macos/WetDelay build/WetDelay.vst3/Contents/arm64-macos/
elif [ -f "artifacts/macos/Contents/MacOS/WetDelay" ]; then
    # Some builds use MacOS instead of arm64-macos
    cp artifacts/macos/Contents/MacOS/WetDelay build/WetDelay.vst3/Contents/arm64-macos/
elif [ -d "artifacts/macos/Release/WetDelay.vst3" ]; then
    # Xcode build outputs to Release/ directory
    echo "Found Xcode build output, copying from Release/..."
    # Copy the binary
    if [ -f "artifacts/macos/Release/WetDelay.vst3/Contents/MacOS/WetDelay" ]; then
        cp artifacts/macos/Release/WetDelay.vst3/Contents/MacOS/WetDelay build/WetDelay.vst3/Contents/arm64-macos/
    fi
else
    echo "WARNING: macOS binary not found"
    echo "Looking for macOS binary..."
    ls -la artifacts/macos/Contents/ 2>/dev/null || echo "macOS artifact directory structure:"
    find artifacts/macos -type f -name "WetDelay" 2>/dev/null || echo "No WetDelay executable found"
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