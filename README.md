Overview
================================================================================
sboui is a package management tool for SlackBuilds.org (SBo). It features an
ncurses user interface that combines the user-friendliness typically found in
a GUI with the efficiency, light weight, and portability of a text interface.
It can function as a stand-alone package manager or serve as a front-end to
sbopkg, sbotools, or custom package managers. Its main features include:

* Efficient operation with the keyboard, and full mouse support
* Forward and inverse dependency resolution
* Automatic rebuilding of inverse dependencies (optional)
* Blacklisting
* Storing build options for individual SlackBuild scripts
* "Tagging" to operate on multiple SlackBuilds
* Searching the repository by name or by content in READMEs
* A menu bar, buttons, drop-downs, and other elements typically found in GUI

Installation
================================================================================
sboui is available from SlackBuilds.org, which is the recommended method for
obtaining it. Of course, custom and local builds are also possible; see the
INSTALL file for more information.

Operation
================================================================================
The interface and operation of sboui was inspired by Midnight Commander, a
two-pane file manager with a text-based interface. If you are familiar with
that application, sboui should be very easy to pick up. 

On first-time use, you may first need to set up the repository. You can do this
with the sync command, which can be found under Actions in the menu bar, using
the 's' keyboard shortcut, or from the command line using the --sync command
line option. You should also run the sync command regularly to check for
updates.

If you already have a local repository set up (for example, if you are using
sboui as a front-end to sbopkg or sbotools), then make sure that repo_dir is set
correctly in the config file. All settings can also be edited interactively in
the Options window (under File in the menu bar or using the 'o' keyboard
shortcut). sboui can also automatically set the relevant config variables when
changing the package manager from the Options window.

To install a package, browse for it in the main window or use a search to locate
it. Search can be found under Actions in the menu bar, or using the '/' keyboard
shortcut. Select the desired package in the right pane and click Enter or
double-click with the mouse. This will bring up a list of actions that can be
performed for that package, including viewing the README, browsing the files for
the SlackBuild in the repository, installing, etc. When a package is installed,
upgraded, reinstalled, or removed, sboui automatically computes the build order
and offers suggestions for each package in the build order. If you do not wish
to perform the suggested action for each of these packages, you can toggle it by
left-clicking in the box or using the space bar.

To upgrade all, you can use the Ctrl-u keyboard combination, look in the menu
bar under Actions, or use the --upgrade-all command line option. This action is
a convenience method which does the following: filter by upgradable SlackBuilds,
tag all, and then upgrade tagged. The same can be done manually if desired.
To tag, use the 't' keyboard shortcut with any SlackBuild highlighted in most
display lists in sboui, or, alternatively, right-click with the mouse. Entire
groups can be tagged by tagging an entry in the Groups list. Filters can be
selected in the menu bar or with the 'f' keyboard shortcut.

This has been a brief introduction to sboui's capabilities and usage. Please
take a look at the keyboard and mouse shortcuts under Help in the menu as well
well as the man pages, and have fun!

Using sboui with custom package managers
================================================================================
sboui can be used with custom SBo package management tools, instead of the
built-in package manager (sboui-backend), sbopkg, or sbotools. The process is
fairly simple, but there are a few requirements:

* The package manager must store a local copy of the SlackBuilds.org repository.
  Use the repo_dir config variable to point to the top level.
* It must implement a sync / update command, referenced by sync_cmd.
* It must implement install, upgrade, and reinstall commands. In general, these
  can all actually be the same command, as long as it handles all these use
  cases. These commands should not resolve dependencies, because sboui handles
  that part. sboui will tack on the name of the package to be installed at the
  end of these commands (install_cmd, upgrade_cmd, and reinstall_cmd). For
  example, install_cmd for sbopkg is "sbopkg -B -i".

In addition to the above, the following are recommended for optimal user
experience:

* When there is an error, the package manager should exit with a return value
  other than 0. Otherwise, sboui will indicate that the command succeeded when
  it actually failed.
* No prompts or questions should be presented to the user by any of these
  commands, because sboui will do the same, resulting in tedious duplicated
  prompts.

Screenshots
================================================================================
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/filters.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/commander.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/search.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/install.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/options.png)
![alt tag](https://raw.githubusercontent.com/montagdude/sboui/master/screenshots/package_info.png)
