#!/usr/bin/env bash
LIB_CURSES="-lncursesw"
#LIB_CURSES="$(xcurses-config --libs-static --cflags)"

gcc LangtonsAnt/*.c \
    -std=gnu18 -Wpedantic -Wall -Wextra \
    -O2 -DSERIAL_COLORS -lm $LIB_CURSES \
    -o "${1:-LangtonsAnt/LangtonsAnt}"
