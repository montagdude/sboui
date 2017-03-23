sboui is an ncurses-based user interface for sbopkg, sbotools, and similar tools
to build and install software from SlackBuild scripts hosted at SlackBuilds.org
(SBo). Unlike many Slackware system administration tools with dialog-based user
interfaces, sboui is written in C++ utilizing low-level ncurses library calls.
This approach allows much greater flexibility in the design of the user
interface and also makes it very fast. The main features of sboui include:

* A two-pane listing of software groups and names
* Filters: all, installed, upgradable, tagged, and non-dependencies
* Searching the local repository
* Forward and inverse dependency resolution
* Viewing and editing files in the local repository
* Tagging multiple packages (or groups) to apply batch changes
* Support for any custom SBo package manager meeting certain conditions 
* Configurable colors
