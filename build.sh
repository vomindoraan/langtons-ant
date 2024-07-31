#!/usr/bin/env bash
gcc LangtonsAnt/*.c -std=gnu18 -pedantic -O2 -DSERIAL_COLORS -lm -lncursesw -o "${1:-LangtonsAnt/LangtonsAnt}"
#gcc LangtonsAnt/*.c -std=c18 -pedantic -O2 -DSERIAL_COLORS -lm $(xcurses-config --libs-static --cflags) -o "${1:-LangtonsAnt/LangtonsAnt}"
