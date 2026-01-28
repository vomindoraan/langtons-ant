#!/usr/bin/env bash
SRC_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." &>/dev/null && pwd)"
APP="${1:-$SRC_DIR/LangtonsAnt}"
shift
FEATURES="${@-SAVE_ENABLE=1 GALLERY_MODE=0 SERIAL_COLORS=0}"

C_FLAGS="-std=gnu18 -Wpedantic -Wall -Wextra -O3"
L_FLAGS="-lm -lncursesw -flto"
for f in $FEATURES; do
    C_FLAGS+=" -D$f"
done

OPT_CURSES=/usr/local/opt/ncurses
if [ -d $OPT_CURSES ]; then
    C_FLAGS+=" -I$OPT_CURSES/include"
    L_FLAGS+=" -L$OPT_CURSES/lib"
fi

echo "Building '$APP' ($FEATURES)..."
gcc "$SRC_DIR"/*.c $C_FLAGS $L_FLAGS -o "$APP"
