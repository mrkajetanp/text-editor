# Changelog

### All notable changes to the program should be listed in this file

---

#### 7.07.2017

* Testing - Updated the test for moving right

#### 4.07.2017

* Testing - Updated the test for letter insertion
* Testing - Updated the test for moving left

#### 4.07.2017

* Testing - Added test for screen's go\_to\_first\_line function

#### 3.07.2017

* Testing - Updated line & screen init tests

#### 19.06.2017

* Save confirmation window - aborting exit with ^C

#### 17.06.2017

* Save confirmation window functionality
* Save confirmation window showing up properly on quit

#### 16.06.2017

* Started implementing save confirmation window
    * disable bottom info bar
    * make space for confirmation window

#### 13.06.2017

* Removed an unnecessary line being added to every opened file

#### 12.06.2017

* Fixed line's visual cursor not incrementing on tabs
* Refactored file\_save function
* Refactored render\_line function
* Implemented proper file saving

#### 8.06.2017

* Fixed moving right at the edge when bottom line is a wrap (Issue #9)

#### 7.06.2017

* Fixed jumping over a line when moving down with top line wrapped (Issue #8)

#### 6.06.2017

* Fixed s->row becoming negative when moving the render up with top line wrapped
* Fixed rendering with multiple wrapped lines
* Fixed adjusting current wrap number after moving up/down
* Moving to the first line after reading a file
* Started implementing file handling
* Opening and closing files
* Getting file contents while opening it

#### 5.06.2017

* Implemented storing and moving to a bigger column when moving up
* Fixed stored column on moving down
* Implemented storing and moving to a bigger column when moving down

#### 2.06.2017

* Fixed wrapping lines by inserting chars in the middle (Issue #5)

#### 1.06.2017

* Fixed blinking cursor sometimes showing up in windows other than contents
* Added visual_cursor to the Line struct
* Fixed cursor after moving lines down when the top line is wrapped 

#### 31.05.2017

* Fixed line wrapping in release mode (Issue #4)

#### 30.05.2017

* Added bottom bar with useful editing information
* Added top bar with basic nano-like information

#### 29.05.2017

* Changed most counter variables to uint
* Added uint typedef to all header files
* Changed comment style on screen's struct fields to be more concise
* Fixed pressing enter at the last visual column in debug mode

#### 27.05.2017

* Implemented proper wrap handling for movement up
* Implemented proper wrap handling for movement down

#### 26.05.2017

* Implemented properly rendering line numbers with wraps
* Fixed right movement to properly handle wrapped lines
* Fixed left movement to properly handle wrapped lines

#### 25.05.2017

* Added wrap field to the line struct
* Attempts to fix movement with wrapped lines

#### 24.05.2017

* Implemented moving right at the end (down) through the wraps of the wrapped line
* Implemented moving left at the beginning (up) through the wraps of the wrapped line

#### 23.05.2017

* Changed move right handling to consider wrapping lines

#### 22.05.2017

* Added wraps field to the line struct
* Added cols field to the screen struct
* Fixed visual cursor position when wrapping lines
* Started handling line wraps

#### 20.05.2017

* When moving up/down onto a tab, cursor now ends up before it

#### 19.05.2017

* Implemented moving lines up using backspace at top left corner
* Widened line numbers bar to 5 characters (4 digits + space)
* Added a field in screen containing the number of visual rows
    * Synced with ncurses's LINES but independent
* added testing field cur\_l\_num to test\_split\_line

#### 18.05.2017

* Switched screen\_new\_line\_under and screen\_new\_line\_above to cur\_l\_num instead of s->row
    * Regarding issue #1
* Implemented moving up and down with more lines using right/left arrow
    * Regarding issue #1
* Implemented moving down with more lines (down arrow key)
    * Regarding issue #1

#### 17.05.2017

* Implemented moving up with more lines
    * Regarding issue #1

#### 16.05.2017

* Started implementing handling of number of lines exceeding the window
	* Enter at the bottom properly adds a new line and moves down the render


#### 14.05.2017

* Added tests for merging the line

#### 13.05.2017

* Added tests for splitting the line

#### 12.05.2017

* Added tests for the backspace key

#### 11.05.2017

* Added tests for the tab key

#### 9.05.2017

* Added tests for the enter key
* Added tests for moving down
* Added tests for moving up

#### 8.05.2017

* Added tests for moving right
* Added tests for moving left

#### 7.05.2017

* Moved entire editor's logic into editor library
* Added tests for letter insertion

#### 6.05.2017

* Added tests for initialization - test\_line\_init
* Added tests for line management - test\_new\_line\_under & test\_new\_line\_above

#### 5.05.2017

* Fixed splitting the line when visual cursor was at the start of the gap
* Fixed visual end when merging lines containing tabs

#### 4.05.2017

* Added unit testing - logic\_test - general - initialization - screen_init
* Separated screen\_ init and screen\_init\_ncurses
* Moved screen into a separate library

#### 3.05.2017

* Fixed visual_end when splitting line containing multiple tabs
* Added separate function inserting a line above to improve performance
* Fixed debug render cursor char according to the gap
* Refactored some input handling functions to have more useful names:
	* handle\_key\_right => handle\_move\_right 

#### 2.05.2017

* Added a bar separating debug info from content
* Moved debug mode information into a separate window

#### 1.05.2017

* Refactored debug mode special characters rendering into a switch statement

#### 30.04.2017

* Added proper resize handling
* Igoring unhandled special characters

#### 29.04.2017

* Rendering - added line numbers
* Fixed cursor movement when merging lines by backspace (tab-related)
* Added color to tabs in debug mode
* Added TASKS.md
