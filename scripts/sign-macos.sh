#!/bin/bash
# Signs macOS bundles with the Developer ID, notarises them with Apple and
# staples the ticket, so they open without a Gatekeeper prompt.
#
#   scripts/sign-macos.sh build/WetCompressor.vst3 build/WetCompressor.component
#
# Needs these environment variables (repository secrets):
#   MACOS_CERT_P12       base64 of the Developer ID Application .p12
#   MACOS_CERT_PASSWORD  its password
#   APPLE_TEAM_ID
#   NOTARY_KEY_P8        App Store Connect API key (.p8 contents)
#   NOTARY_KEY_ID
#   NOTARY_ISSUER_ID
set -euo pipefail

if [ -z "${MACOS_CERT_P12:-}" ]; then
    echo "::error::MACOS_CERT_P12 is not set - cannot sign"
    exit 1
fi

WORK="$RUNNER_TEMP/signing"
mkdir -p "$WORK"
KEYCHAIN="$WORK/signing.keychain-db"
KEYCHAIN_PW=$(openssl rand -base64 24)

# A throwaway keychain holding only the certificate, deleted by the runner.
echo "$MACOS_CERT_P12" | base64 --decode > "$WORK/cert.p12"
security create-keychain -p "$KEYCHAIN_PW" "$KEYCHAIN"
security set-keychain-settings -lut 3600 "$KEYCHAIN"
security unlock-keychain -p "$KEYCHAIN_PW" "$KEYCHAIN"
security import "$WORK/cert.p12" -k "$KEYCHAIN" -P "$MACOS_CERT_PASSWORD" \
    -T /usr/bin/codesign
security set-key-partition-list -S apple-tool:,apple: -s -k "$KEYCHAIN_PW" "$KEYCHAIN" > /dev/null
security list-keychains -d user -s "$KEYCHAIN" $(security list-keychains -d user | tr -d '"')
rm "$WORK/cert.p12"

IDENTITY="Developer ID Application: Ronald Klarenbeek ($APPLE_TEAM_ID)"

sign() {
    codesign --force --timestamp --options runtime --keychain "$KEYCHAIN" \
        --sign "$IDENTITY" "$1"
}

# Inside out: a bundle nested in another must be signed before its parent.
for BUNDLE in "$@"; do
    while IFS= read -r NESTED; do
        echo "Signing nested $NESTED"
        sign "$NESTED"
    done < <(find "$BUNDLE/Contents" -depth -type d \( -name "*.vst3" -o -name "*.component" \))
    echo "Signing $BUNDLE"
    sign "$BUNDLE"
    codesign --verify --deep --strict --verbose=2 "$BUNDLE"
done

# One submission for all bundles.
echo "$NOTARY_KEY_P8" > "$WORK/notary.p8"
mkdir -p "$WORK/stage"
for BUNDLE in "$@"; do ditto "$BUNDLE" "$WORK/stage/$(basename "$BUNDLE")"; done
ditto -c -k --sequesterRsrc "$WORK/stage" "$WORK/notarise.zip"
xcrun notarytool submit "$WORK/notarise.zip" \
    --key "$WORK/notary.p8" --key-id "$NOTARY_KEY_ID" --issuer "$NOTARY_ISSUER_ID" \
    --wait --timeout 30m --output-format json | tee "$WORK/notary.json"
rm "$WORK/notary.p8"

STATUS=$(python3 -c "import json,sys; print(json.load(open(sys.argv[1]))['status'])" "$WORK/notary.json")
if [ "$STATUS" != "Accepted" ]; then
    ID=$(python3 -c "import json,sys; print(json.load(open(sys.argv[1]))['id'])" "$WORK/notary.json")
    echo "$NOTARY_KEY_P8" > "$WORK/notary.p8"
    xcrun notarytool log "$ID" --key "$WORK/notary.p8" \
        --key-id "$NOTARY_KEY_ID" --issuer "$NOTARY_ISSUER_ID" || true
    rm "$WORK/notary.p8"
    echo "::error::Notarisation status: $STATUS"
    exit 1
fi

for BUNDLE in "$@"; do
    xcrun stapler staple "$BUNDLE"
    xcrun stapler validate "$BUNDLE"
    # Gatekeeper's own verdict. A plug-in is not an app, so this is the
    # "open" assessment a DAW triggers when it loads the bundle.
    spctl --assess --type open --context context:primary-signature --verbose=2 "$BUNDLE"
done

security delete-keychain "$KEYCHAIN"
echo "Signed, notarised and stapled: $*"
