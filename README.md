# Langton's Ant

Cross-platform simulator for a 14-color [Langton's ant](https://en.wikipedia.org/wiki/Langton%27s_ant) 2D Turing machine (or, more precisely, a [turmite](https://en.wikipedia.org/wiki/Turmite)).

The simulator supports dynamically changing the ant's ruleset (colors and turn directions) in real time, which often results in interesting patterns and emergent behavior.

Written in C. Uses ncurses on Linux/macOS and [PDCurses](https://github.com/wmcbrine/PDCurses) on Windows for character graphics.

Initially written as an assignment for the Numerical Analysis and Discreet Mathematics (SI2NAD) course at the University of Belgrade, School of Electrical Engineering (ETF).
Partly inspired by this classic [video](https://www.youtube.com/watch?v=1X-gtr4pEBU).

This software was exhibited as part of the [_Pomeraj u kôdu: A Sandbox for Ants_](https://suluv.org/pomeraj-u-kodu-3/) art project in SULUV, Novi Sad, Serbia (2024).

![Screenshot](https://github.com/vomindoraan/langtons-ant/blob/master/Misc/screenshot_ui.png?raw=true)


## Building and running

### Linux, macOS

From the source directory `LangtonsAnt`, run the following commands in a shell:

```sh
# Build the project (outputs to ./LangtonsAnt by default)
scripts/build.sh

# Alternatively, output to specified path with optional feature flags
scripts/build.sh /usr/bin/lant SAVE_ENABLE=1 GALLERY_MODE=0 SERIAL_COLORS=1

# Run the project (optional: path)
# Works best with lxterminal, but any curses-capable POSIX terminal will work
scripts/run.sh #/usr/bin/lant
```

### Windows

Open `langtons-ant.sln` in Visual Studio, build the solution for the desired platform (Win32, x64) and run it.

Binary distributions are output to the default VS build folders. Feature flags can be set in _Project Properties_ ⟩ _C/C++_ ⟩ _Preprocessor Definitions_, or by editing the `LangtonsAnt.vcxproj` file.

### Teensy++2.0 (Arduino)

In the `Teensy` directory, there's optional support for an external, Arduino-controlled array of TFT screens that display the ant's color rules in real time.
(Requires building the main project with `SERIAL_COLORS=1`.)


## Documentation

See [Docs/README.md](Docs/README.md).

Clicking on the logo inside the program will display a controls cheat sheet.
