#!/usr/bin/env bash
# Reproduce the TransFuzz "MiRTL" Yosys build from upstream YosysHQ/yosys
# pinned to commit 3c3788ee2 (Yosys 0.37+29) plus the TransFuzz overlay.
#
# Usage:
#   bash scripts/build-yosys.sh           # build only
#   JOBS=8 bash scripts/build-yosys.sh    # control parallelism
#
# Installation (`make install`) requires root and is intentionally NOT
# performed by this script. Run `sudo make -C yosys install` afterwards
# if you want the binary system-wide.

set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
YOSYS="$ROOT/yosys"
OVERLAY="$ROOT/yosys-overlay"
PIN="3c3788ee2"
JOBS="${JOBS:-$(nproc)}"

if [ ! -e "$YOSYS/.git" ]; then
    echo "error: $YOSYS is not a git checkout. Run 'git submodule update --init' first." >&2
    exit 1
fi

echo ">>> Resetting $YOSYS to upstream commit $PIN"
git -C "$YOSYS" fetch --tags --quiet origin || true
git -C "$YOSYS" reset --hard "$PIN"
git -C "$YOSYS" clean -fdx

echo ">>> Applying TransFuzz overlay"
git -C "$YOSYS" apply --whitespace=nowarn "$OVERLAY/kernel-rtlil.patch"
mkdir -p "$YOSYS/passes/simufuzz"
cp -f "$OVERLAY"/passes/simufuzz/* "$YOSYS/passes/simufuzz/"

echo ">>> Building Yosys with $JOBS jobs"
make -C "$YOSYS" -j"$JOBS"

echo
echo ">>> Done. Binary: $YOSYS/yosys"
echo ">>> To install system-wide: sudo make -C $YOSYS install"
