#!/usr/bin/env bash
# build.sh — Build & run Cerberus Dashboard Simulator
# Usage:
#   ./build.sh          → build + run
#   ./build.sh build    → build only
#   ./build.sh run      → run only (skip rebuild)
#   ./build.sh clean    → hapus build folder

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
BINARY="$SCRIPT_DIR/dashboard_sim"

# ── Warna output ──────────────────────────────────────────────────────────────
RED='\033[0;31m'; GREEN='\033[0;32m'; CYAN='\033[0;36m'; NC='\033[0m'

log()  { echo -e "${CYAN}[SIM]${NC} $*"; }
ok()   { echo -e "${GREEN}[OK]${NC}  $*"; }
err()  { echo -e "${RED}[ERR]${NC} $*"; }

# ── Clean ─────────────────────────────────────────────────────────────────────
if [[ "$1" == "clean" ]]; then
    log "Menghapus build folder..."
    rm -rf "$BUILD_DIR" "$BINARY"
    ok "Clean selesai."
    exit 0
fi

# ── Build ─────────────────────────────────────────────────────────────────────
do_build() {
    log "Checking dependencies..."

    if ! command -v cmake &>/dev/null; then
        err "cmake tidak ditemukan. Install: sudo apt install cmake"
        exit 1
    fi

    if ! pkg-config --exists sdl2 2>/dev/null; then
        err "libsdl2-dev tidak ditemukan. Install: sudo apt install libsdl2-dev"
        exit 1
    fi

    log "CMake configure..."
    mkdir -p "$BUILD_DIR"
    cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

    log "Building... (download LVGL jika pertama kali — butuh internet)"
    cmake --build "$BUILD_DIR" --parallel "$(nproc)"

    ok "Build sukses → $BINARY"
}

# ── Run ───────────────────────────────────────────────────────────────────────
do_run() {
    if [[ ! -f "$BINARY" ]]; then
        err "Binary tidak ditemukan. Jalankan: ./build.sh build"
        exit 1
    fi

    log "Menjalankan simulator..."
    echo ""
    echo "  Keys:"
    echo "    0 = data off    1 = RPM sweep"
    echo "    2 = idle        3 = driving    4 = redline"
    echo "    Q / ESC = quit"
    echo ""
    "$BINARY"
}

# ── Dispatch ──────────────────────────────────────────────────────────────────
case "${1:-all}" in
    build) do_build ;;
    run)   do_run   ;;
    all)   do_build && do_run ;;
    *)
        echo "Usage: $0 [build|run|clean]"
        exit 1
        ;;
esac
