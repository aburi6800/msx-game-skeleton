[ [Engligh](README.md) | [日本語](README.ja.md) ]

---

# MSX Game Program Templete

</br>

## Overview

- This is a template project for creating game programs that run on MSX.
- It uses z88dk to compile and create ROM files.
- Write the state required for each game in GameStatus_t in game.h, and define any other necessary structures.
- Write each game's processing in game.c and the screen drawing processing in the draw_game function.
- The timing for calling the draw_game function is specified by FRAME_RATE in const.h. (0=1/60, 1=1/30, 2=1/15)

</br>

## Supported Features

- The following features are supported:
  - Drawing to the screen buffer
  - Screen drawing via H.TIMI interrupts
  - Input acquisition via keyboard/joystick
  - Sprite plane shuffle
  - 16-direction collision detection between two objects

</br>

## Unsupported Features

- The following features are unsupported:
  - Sound drivers
  - Keyboard B button input
  - Scroll processing

</br>

## Release notes

ver.20260117

- Prototype Version

</br>

## License

MIT License

</br>

## Thanks

- [Z88DK - The Development Kit for Z80 Computers](https://github.com/z88dk/z88dk)
- [C-BIOS](http://cbios.sourceforge.net/)
- [openMSX](https://openmsx.org/)
