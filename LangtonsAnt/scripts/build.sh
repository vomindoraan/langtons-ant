#!/usr/bin/env bash
SRC_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." &>/dev/null && pwd)"

FEATURES="-DSERIAL_COLORS_ON"  # SERIAL_COLORS_ON, GALLERY_MODE_ON

C_FLAGS="-std=gnu18 -Wpedantic -Wall -Wextra -O3"
LIB_CURSES="-lncursesw"
#LIB_CURSES="$(xcurses-config --libs-static --cflags)"
L_FLAGS="-lm $LIB_CURSES -flto"

OPT_CURSES=/usr/local/opt/ncurses
if [ -d $OPT_CURSES ]; then
    C_FLAGS+=" -I$OPT_CURSES/include"
    L_FLAGS+=" -L$OPT_CURSES/lib"
fi

gcc "$SRC_DIR"/*.c $FEATURES $C_FLAGS $L_FLAGS -o "${1:-LangtonsAnt}"
