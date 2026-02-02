## Description

A project I made for fun which is just the famous Minesweeper but inside your terminal emulator.

A lot of the code this project runs on is limited to libraries and core utilities of the Linux OS (in general), so I'm not sure how one would get this working on any other Operating Systems...

## Gameplay

I'm sure the way Minesweeper is played is widely known at this point but it wouldn't hurt to explain it again.

You are presented with a grid of tiles under which could either be a mine or nothing. Your goal is to reveal every tile that is not a mine.

To start, navigate to a random tile with the **W, A, S, D** keys and hit **SPACE** to reveal it, if the tile was a mine, tough luck, if it was not, the tile, upon getting revealed, will then represent the number of mines adjacent to it (vertically, horizontally and diagonally) with a number (or it will display nothing if there are no mines around).

Using logic you will have to figure out where mines are located and try not to reveal them to win the game.

If you are sure a mine is under an unrevealed tile, you can flag/unflag that tile with the **F** key which will prevent that tile from getting revealed by accident.

## Building / Compiling

This project makes use of the GCC compiler and the "Make" standart UNIX utility through the [Makefile](/Makefile) for an easier time when compiling the project.

If you're using Linux, you're in luck because the game was made for that platform. To build the project on Linux, simply navigate into the directory where this project resides and make use of the following commands:

- `make` to create an executable file.
- `make run` to run the aforementioned executable. (And rebuild the project if changes have been detected in the source files)
- `make clean` to delete the executable.

If you're not using Linux. I'm sorry but, at least for now, you're on your own here.

## Dependencies

If the "ENABLE_AUDIO" option has been enabled in the [main.c](/main.c) file, you'll need to have the [aplay](https://linux.die.net/man/1/aplay) utility installed.

As far as I know that's the only problem you might encounter when building the thing.

## Configuration?

In the [main.c](/main.c) file there's a bunch of definitions that you can change at your leisure, some of them are:

These should be integers:
- GRID_W -> The width of the game grid.
- GRID_H -> The height of the game grid.
- N_MINES -> The number of mines that will be present on the game grid. (The number might be off by one, I forgot to count)

These can be either true or false:
- ENABLE_AUDIO -> self explanatory.
- ENABLE_COLOURS -> Same here.

There's also the colour definitions there which are just [escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors) that get printed to the terminal and set the colour.

Below the definitions you can find enums for some keybinds but I'm sure you can figure out how to change them if you happen to want to do that. You're so smart.

## Fin

I've done no testing outside of my own machine. Fun!

Here have an image of the gameplay, as a treat.

![game](/misc/example.png)
