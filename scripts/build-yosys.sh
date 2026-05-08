#!/usr/bin/env bash
# Build Yosys from the m2kar/mirtl-yosys fork (simufuzz branch).
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
JOBS="${JOBS:-$(nproc)}"

if [ ! -e "$YOSYS/.git" ]; then
    echo "error: $YOSYS is not a git checkout. Run 'git submodule update --init' first." >&2
    exit 1
fi

echo ">>> Building Yosys with $JOBS jobs (mirtl-yosys fork)"
make -C "$YOSYS" -j"$JOBS"

echo
echo ">>> Done. Binary: $YOSYS/yosys"
echo ">>> To install system-wide: sudo make -C $YOSYS install"
