#!/usr/bin/env bash
FEATURES="-DSERIAL_COLORS_ON"

C_FLAGS="-std=gnu18 -Wpedantic -Wall -Wextra -O2"

LIB_CURSES="-lncursesw"
#LIB_CURSES="$(xcurses-config --libs-static --cflags)"
L_FLAGS="-lm $LIB_CURSES"

gcc LangtonsAnt/*.c $FEATURES $C_FLAGS $L_FLAGS -o "${1:-LangtonsAnt/LangtonsAnt}"
