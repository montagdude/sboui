Overview
================================================================================
sboui is an ncurses-based user interface for sbopkg, sbotools, and similar tools
to build and install software from SlackBuild scripts hosted at SlackBuilds.org
(SBo). Unlike many Slackware system administration tools with dialog-based user
interfaces, sboui is written in C++ utilizing low-level ncurses library calls.
This approach allows much greater flexibility in the design of the user
interface and also makes it very fast. The main features of sboui include:

* Two-pane browsable listing of software groups and names
* Filters: all, installed, upgradable, tagged, and non-dependencies
* Search the repository
* Dependency resolution: forward and inverse mode
* View and edit files in the repository
* Tag multiple packages (or groups) to apply batch changes
* Comes with several built-in color themes and supports custom themes
* Designed for and tested to work flawlessly with sbopkg and sbotools, but it is
  easy to set it up to work with your own custom package manager

Installation
================================================================================
sboui is available from SlackBuilds.org, so you can install it from there using
the SlackBuild script or your favorite package manager. If you want to do a
custom build, see the instructions in the included file called INSTALL.

Use
================================================================================
The basic concept of sboui is to list software from SlackBuilds.org using a two-
pane listing organized by groups. Switch between the right and left panes using
the arrow keys or Tab, and navigate within a pane using the arrow keys, Home,
End or PageUp/PageDown.

The first thing you normally will want to do after starting sboui is to sync the
local repository with the official remote SlackBuilds.org repository. This
ensures that sboui has the latest information about available software and
versions. The s key performs the sync operation, displays the result on the
screen, and then lets you return to the main window.

After you have highlighted the name of some software of interest in the right
pane, use the Enter key to see possible actions (e.g., view the README, install,
upgrade, remove, compute the build order, or browse files). Choose an action by
highlighting the desired entry and pressing Enter, or use a hotkey. When you
choose any action that modifies installed software on your computer, sboui
computes the dependencies recursively in the proper order to build it and lets
you choose whether to take action on each one. Building something with lots of
dependencies is easy; just select it, choose "Install," and press Enter to build
and install it and all its dependencies. You can also interactively choose to
skip some dependencies or turn off dependency resolution completely if desired.

sboui also provides filters. When you start the program, all SlackBuilds in the
repository will be listed. The f key allows you to select from several filters
to show only those SlackBuilds meeting certain criteria (e.g., installed,
upgradable, tagged, and others). You can also filter by a search criterion with
the / key.

It's often desired to apply changes in batch. For example, you may want to
upgrade everything on your system that you have installed from SlackBuilds.org.
In sboui, this is done by "tagging." Press t to tag the highlighted software
or the entire group (if done in the left pane). Then, use keyboard shortcuts to
apply some action to everything that is tagged. For example, "i" installs
everything that is tagged, and "u" upgrades everything that is tagged. sboui is
smart enough to only apply the action when possible -- it will not try to
upgrade something that you have tagged which is not upgradable, for example.

This is just a brief overview of how to use sboui, but there is much more that
it can do. Take a look at the available keyboard shortcuts (by pressing the ?
key) and have fun!

Using sboui with custom package managers
================================================================================
sboui is designed for and tested to work with sbopkg and sbotools, but it is
also easy to use it with custome package managers or scripts. There are just a
few requirements:

* The package manager must store a local copy of the SlackBuilds.org repository
  with a directory structure as follows: top level (includes subdirectories for
  each group) -> group (includes subdirectories for each software entry in the
  group) -> software (includes the SlackBuild script and related files to
  download and build the software).
* The package manager must implement a sync command (sync_cmd) to sync the local
  repository with the official SlackBuilds.org repository.
* The package manager must implement an install command (install_cmd) to build
  and install the software package. sboui issues the install command in the
  following form: `install_vars install_cmd {software name} install_clos`, where
  install_vars are environment variables passed to the package manager and
  install_clos are additional user-specified command-line options. For example,
  for sbopkg, install_cmd is "sbopkg -B -i" by default. If the package manager
  normally resolves dependencies, the install command should disable this
  behavior, because sboui resolves dependencies natively. The install command
  should build and install *only* the software identified by {software name}.
* The package manager must implement an upgrade command to build and upgrade
  the software package. In general, this command has the same requirements as
  the install command, and the two commands can actually be the same (such is
  the case with sbopkg), unless the package manager doesn't permit the install
  command to also be used for upgrades.

In addition to what is listed above, each of the above commands should ideally
do the following for the best user experience:

* When there is an error, the package manager should exit with an error code
  (something other than 0). This allows sboui to know that the command failed
  and display an error message. Otherwise, sboui may indicate the command
  succeeded when it actually did not.
* These commands should not present the user with any prompts or questions. (If
  they do by default, this behavior should be disabled in the specification of
  sync_cmd, install_cmd, and upgrade_cmd when used with sboui.) Otherwise,
  there may be duplicate prompts from sboui and the package manager.

Screenshots
================================================================================
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/filters.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/commander.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/search.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/install.png)
