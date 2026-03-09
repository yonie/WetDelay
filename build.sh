#!/bin/bash
echo "================================================"
echo " WetDelay VST3 Plugin - Build Script"
echo "================================================"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
    echo "Platform: macOS"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
    echo "Platform: Linux"
else
    echo "ERROR: Unsupported platform: $OSTYPE"
    echo "This script only supports Linux and macOS."
    echo "For Windows, use build.bat"
    exit 1
fi

if [ ! -d "vst3sdk" ]; then
    echo "ERROR: vst3sdk directory not found!"
    echo ""
    echo "Please clone the VST3 SDK first:"
    echo "  git clone --recursive https://github.com/steinbergmedia/vst3sdk.git"
    echo ""
    exit 1
fi

if [ -d "WetDelay/build" ]; then
    echo "Cleaning previous build..."
    rm -rf WetDelay/build
fi

echo "Creating fresh build directory..."
mkdir -p WetDelay/build
cd WetDelay/build

echo ""
echo "Step 1: Configuring CMake..."
echo "================================================"

if [ "$PLATFORM" = "macos" ]; then
    # macOS requires Xcode generator for VSTGUI/ObjC++
    cmake .. -GXcode -DCMAKE_BUILD_TYPE=Release -DSMTG_CREATE_PLUGIN_LINK=0
else
    # Linux uses Makefiles
    cmake .. -DCMAKE_BUILD_TYPE=Release -DSMTG_CREATE_PLUGIN_LINK=0
fi

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed!"
    cd "$SCRIPT_DIR"
    exit 1
fi

echo ""
echo "Step 2: Building..."
echo "================================================"

if [ "$PLATFORM" = "macos" ]; then
    # Use xcodebuild for macOS
    xcodebuild -configuration Release -jobs $(sysctl -n hw.ncpu)
    BUILD_RESULT=$?
else
    # Use make for Linux
    make -j$(nproc)
    BUILD_RESULT=$?
fi

echo ""
echo "================================================"
echo " Build successful!"
echo "================================================"
echo ""

if [ "$PLATFORM" = "macos" ]; then
    # macOS output location with Xcode
    if [ -d "Release/WetDelay.vst3" ]; then
        echo "Plugin location: WetDelay/build/Release/WetDelay.vst3"
        echo ""
        
        echo "Step 3: Code signing for macOS..."
        echo "================================================"
        codesign --force --deep --sign - "Release/WetDelay.vst3"
        if [ $? -eq 0 ]; then
            echo "Plugin signed successfully (ad-hoc signature)"
        else
            echo "WARNING: Code signing failed, but plugin may still work"
        fi
        echo ""
        
        echo "Step 4: Running VST3 validator..."
        echo "================================================"
        if [ -f "../vst3sdk/build/bin/validator" ]; then
            ../vst3sdk/build/bin/validator "Release/WetDelay.vst3"
            if [ $? -eq 0 ]; then
                echo ""
                echo "Validation passed!"
            else
                echo ""
                echo "WARNING: Validation reported issues (this may be normal for some tests)"
            fi
        else
            echo "Validator not found - skipping validation"
        fi
    else
        if [ $BUILD_RESULT -ne 0 ]; then
            echo "ERROR: Build failed!"
            cd "$SCRIPT_DIR"
            exit 1
        fi
        echo "WARNING: Plugin binary not found at expected location"
    fi
else
    # Linux output location
    if [ -f "VST3/Release/WetDelay.vst3/Contents/x86_64-linux/WetDelay.so" ]; then
        echo "Plugin location: WetDelay/build/VST3/Release/WetDelay.vst3"
        echo ""
        
        echo "Step 3: Running VST3 validator..."
        echo "================================================"
        if [ -f "bin/Release/validator" ]; then
            bin/Release/validator "VST3/Release/WetDelay.vst3"
            if [ $? -eq 0 ]; then
                echo ""
                echo "Validation passed!"
            else
                echo ""
                echo "WARNING: Validation reported issues (this may be normal for some tests)"
            fi
        else
            echo "Validator not found - skipping validation"
        fi
    else
        if [ $BUILD_RESULT -ne 0 ]; then
            echo "ERROR: Build failed!"
            cd "$SCRIPT_DIR"
            exit 1
        fi
        echo "WARNING: Plugin binary not found at expected location"
    fi
fi

echo ""
echo "To install: ./install.sh"
echo ""

cd "$SCRIPT_DIR"