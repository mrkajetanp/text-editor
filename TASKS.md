# Currently awaiting development tasks

## Features

### Top priority

* Wrap lines longer than the width of the window
* In debug mode, wrap lines before debug information
* Release rendering - useful information
* File handling

### Future ideas

* Remember the last column position when moving up/down in the way emacs does
* Allow variable tab length
* Allow multiple buffers
* Allow splitting the screen (use ncurses window functionality)
* Allow a buffer to have no lines?
* Completely separate rendering from backend

## Bux Fixes

* Handle number of lines longer than window height

## Improvements

* Refactor debug special characters rendering into a switch statement
* Separate function when handling enter at the beginning of the line
