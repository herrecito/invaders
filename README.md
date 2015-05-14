## Description

A Space Invaders / Intel 8080 emulator in C, using SDL.

![invaders](http://i.imgur.com/q4Y5Vae.gif)

The code should be really easy to understand to anyone wanting to learn how to
create its own emulator.

To run it:

    make
    ./invaders

The code expecs the ROM to be a single file called "invaders.rom". If you find
the ROM splitted in four files, just `cat` them:

    cat invaders.h invaders.g invaders.f invaders.e > invaders.rom

## Known issues

The DAA instruction and the AC flag are not implemented, but are only used to
display the credits so the game its fully playable.

## TODO

* Add fullscreen mode
* Get source and destination from opcodes
* Finish input
* Implement DAA and AC flag
* Finish all other instructions
* Try other ROMs
* Add sound
* Add "color"

## Useful links

* [Emulator 101](http://emulator101.com/)
* [Computer Archeology](http://www.computerarcheology.com/wiki/wiki/Arcade/SpaceInvaders)
* [JavaScript 8080 Emulator](http://bluishcoder.co.nz/js8080/)
* [SIEMU](http://www.darkpact.com/proj/siemu/)
* [Wikipedia - Intel 8080](http://en.wikipedia.org/wiki/Intel_8080)

Also, google the Intel 8080 manual.
