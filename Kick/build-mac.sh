#!/bin/bash
# Configure and build the KICK app (Kick target) on macOS. See BUILDING_ON_MAC.md.
# Needs CMake and Ninja on PATH (or set CMAKE / NINJA), and network access the first time
# to fetch JUCE 7.0.12 and CPM.
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
CMAKE="${CMAKE:-cmake}"
NINJA="${NINJA:-ninja}"
JUCE_DIR="$ROOT/.3rdPartySourceCache/JUCE-7.0.12"
[ -d "$JUCE_DIR" ] || git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git "$JUCE_DIR"

# JUCE 7.0.12's juceaide calls an API removed in macOS 15; target 12.0 as Kick itself does.
export MACOSX_DEPLOYMENT_TARGET=12.0
"$CMAKE" -S "$ROOT/Kick" -B "$ROOT/Kick/build-sim" -G Ninja -DCMAKE_MAKE_PROGRAM="$(command -v "$NINJA" || echo "$NINJA")" \
    -DCMAKE_BUILD_TYPE=Release -DTIMESTAMP=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCPM_JUCE_SOURCE="$JUCE_DIR" -DCPM_AudIO_SOURCE="$ROOT/AudIO" -DCPM_COM_PROTOCOL_SOURCE="$ROOT/ComProtocol"
"$NINJA" -C "$ROOT/Kick/build-sim" Kick
echo "Built: $ROOT/Kick/build-sim/Kick.app"
