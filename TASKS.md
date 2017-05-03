# Currently awaiting development tasks

## Features

### Top priority

* Wrap lines longer than the width of the window
* Release rendering - useful information
* File handling

### Future ideas

* Properly handle keys with modifiers
* Remember the last column position when moving up/down in the way emacs does
* Allow variable tab length
* Allow multiple buffers
* Allow splitting the screen (use ncurses window functionality)
* Allow a buffer to have no lines?
* Completely separate rendering from backend

## Bux Fixes

* Handle number of lines longer than window height

## Improvements

* When moving up/down onto a tab, cursor should end up before it