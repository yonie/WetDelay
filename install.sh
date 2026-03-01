#!/bin/bash
echo "================================================"
echo " WetDelay VST3 Plugin - Linux Installation"
echo "================================================"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

PLUGIN_SOURCE="WetDelay/build/VST3/Release/WetDelay.vst3"
PLUGIN_DEST="$HOME/.vst3"

if [ ! -d "$PLUGIN_SOURCE" ]; then
    echo "ERROR: Plugin not found at $PLUGIN_SOURCE"
    echo ""
    echo "Please run ./build.sh first to build the plugin."
    echo ""
    exit 1
fi

if [ ! -d "$PLUGIN_DEST" ]; then
    echo "Creating VST3 directory: $PLUGIN_DEST"
    mkdir -p "$PLUGIN_DEST"
fi

echo "Installing WetDelay.vst3 to $PLUGIN_DEST..."
cp -r "$PLUGIN_SOURCE" "$PLUGIN_DEST/"

if [ $? -eq 0 ]; then
    echo ""
    echo "================================================"
    echo " Installation successful!"
    echo "================================================"
    echo ""
    echo "Plugin installed to: $PLUGIN_DEST/WetDelay.vst3"
    echo ""
    echo "Next steps:"
    echo "1. Restart your DAW if it's running"
    echo "2. Scan for new plugins (most DAWs do this automatically)"
    echo "3. Look for WetDelay in your plugin list"
    echo ""
else
    echo ""
    echo "ERROR: Installation failed"
    echo ""
    exit 1
fi

cd "$SCRIPT_DIR"