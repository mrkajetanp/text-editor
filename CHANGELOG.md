# Changelog

### All notable changes to the program should be listed in this file

---

#### 3.05.2017

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
