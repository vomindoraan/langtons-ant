# Langton's Ant

Cross-platform simulator for a 14-color [Langton's ant](https://en.wikipedia.org/wiki/Langton%27s_ant) 2D Turing machine (or, more precisely, a [turmite](https://en.wikipedia.org/wiki/Turmite)).

The simulator supports dynamically changing the ant's ruleset (colors and turn directions) in real time, which can result in interesting patterns and emergent behavior.

Written in C. Uses ncurses on Linux/macOS and [PDCurses](https://github.com/wmcbrine/PDCurses) on Windows for character graphics.

Initially written as an assignment for the Numerical Analysis and Discreet Mathematics (SI2NAD) course at the University of Belgrade, School of Electrical Engineering (ETF).
Partly inspired by this classic [video](https://www.youtube.com/watch?v=1X-gtr4pEBU).

This software was exhibited as part of the [_Pomeraj u kôdu: A Sandbox for Ants_](https://suluv.org/pomeraj-u-kodu-3/) art project in SULUV, Novi Sad, Serbia (2024).

![Screenshot](https://github.com/vomindoraan/langtons-ant/blob/master/Misc/screenshot_ui.png?raw=true)
