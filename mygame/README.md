# Terminal Ball

A small Linux terminal program written in C with ncurses. Move a symbol around
the terminal using the W, A, S, and D keys.

The program reads key press and release events from a Linux evdev input device.
It uses `epoll` to wait for input and `timerfd` to update movement at a fixed
rate, independently of keyboard auto-repeat.

## Features

- Continuous movement while a key is held
- Diagonal movement with simultaneous keys
- Screen-edge wrapping
- Timer-driven movement at a consistent speed
- Event-driven input with low CPU usage while idle

## Requirements

The program is Linux-specific because it uses:

- Linux evdev input devices (`/dev/input/event*`)
- `epoll`
- `timerfd`
- ncurses

On Debian or Ubuntu, install the build dependencies with:

```sh
sudo apt install build-essential libncurses-dev
```

On Fedora:

```sh
sudo dnf install gcc ncurses-devel
```

## Build

Assuming the source file is named `terminal_ball.c`:

```sh
gcc -Wall -Wextra -Wpedantic -O2 \
    terminal_ball.c -o terminal-ball -lncurses
```

The source must include `<stdint.h>` because the timer expiration counter uses
`uint64_t`.

## Input Device

The default input device is selected by `EVENTDEV`:

```c
#ifndef EVENTDEV
#define EVENTDEV "/dev/input/event0"
#endif
```

`/dev/input/event0` may not be the keyboard on every system. Inspect the
available input devices with:

```sh
cat /proc/bus/input/devices
```

Select another device at compile time if necessary:

```sh
gcc -Wall -Wextra -Wpedantic -O2 \
    -DEVENTDEV='"/dev/input/event3"' \
    terminal_ball.c -o terminal-ball -lncurses
```

A stable keyboard path under `/dev/input/by-id/` is preferable when available.

## Permissions

Reading `/dev/input/event*` is commonly restricted. Running the entire program
as root works, but granting narrowly controlled input access is preferable.
Depending on the Linux distribution, the user can be added to the `input`
group:

```sh
sudo usermod -aG input "$USER"
```

Log out and back in for the new group membership to take effect.

Raw keyboard access can expose keys entered in other applications. Do not grant
this access to untrusted users or programs.

## Run

```sh
./terminal-ball
```

## Controls

| Key | Action |
| --- | --- |
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| Multiple movement keys | Move diagonally |
| `Q` | Quit when the key is released |

Opposite directions cancel each other. For example, holding `A` and `D`
together produces no horizontal movement.

## How It Works

The program monitors two file descriptors with `epoll`:

1. The evdev input descriptor reports key presses, repeats, and releases.
2. A periodic `timerfd` controls movement updates.

Input events update the current key states. At each timer expiration, the
program calculates horizontal and vertical movement from those states and
moves the symbol by one terminal cell.

Linux key-event values are:

| Value | Meaning |
| --- | --- |
| `0` | Released |
| `1` | Pressed |
| `2` | Auto-repeat |

Press and auto-repeat events keep a key active. A release event clears its
state.

## Configuration

The movement interval is configured in milliseconds:

```c
#define MOVE_INTERVAL_MS 50
```

A 50 ms interval produces 20 movement updates per second. Lower values increase
the update rate and terminal output traffic.

Change the displayed symbol with:

```c
#define DFL_SYMBOL 'o'
```

## Terminal Rendering

### Occasional Flicker

While the symbol is moving continuously, it may occasionally disappear for a
fraction of a frame and then reappear at its next position. This is most
noticeable when holding a movement key such as `D`. The movement state and
timer can still be correct when this happens; the symbol has not been removed
from the game state.

Each movement changes two terminal cells:

1. Curses replaces the symbol at its old position with a blank.
2. Curses draws the symbol at its new position.

`refresh()` batches both changes in curses' logical screen, but the terminal
ultimately receives a sequence of output bytes. The physical terminal may
repaint after processing the erase operation but before processing the draw
operation. That intermediate frame contains no visible symbol and appears as a
brief flicker. Standard terminal output cannot guarantee that two separate
cells are physically updated at the same instant.

Reducing `MOVE_INTERVAL_MS` increases the number of screen updates per second.
For example, changing it from 50 ms to 33 ms increases the update rate from 20
to about 30 frames per second. The additional output gives the terminal more
opportunities to display an intermediate frame, so a faster timer can make the
flicker more visible rather than making the animation smoother.

This explanation applies when the symbol disappears only briefly and returns
on a following frame. If it remains missing for multiple timer updates, the
cause is probably a program error rather than terminal rendering.

Practical mitigations include:

- Keep the movement interval around 50 to 67 ms.
- Use DEC synchronized output on a terminal that supports it.
- Draw the new symbol before erasing the old one, trading a missing-symbol
  frame for the possibility of briefly displaying two symbols.
- Reduce other terminal output while the animation is running.
- Use a graphical or framebuffer API with synchronized presentation when
  completely tear-free animation is required.

## Limitations

- Linux only
- Requires permission to read the selected input device
- Does not explicitly handle terminal resize events
- Diagonal movement covers more geometric distance per tick
- Terminal output cannot portably update two cells atomically

## Possible Extensions

- Collectible targets and scoring
- Walls and collision detection
- Enemies and pathfinding
- Projectiles or a dash action
- Levels with increasing difficulty
- Pause, restart, and game-over states
- Terminal resize handling
- A status window showing score and elapsed time

