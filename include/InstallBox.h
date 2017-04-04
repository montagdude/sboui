#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "BuildOrderBox.h"

/*******************************************************************************

Allows user to install/upgrade/reinstall/remove SlackBuild and dependencies

*******************************************************************************/
class InstallBox: public BuildOrderBox {

  private:

    /* For this class, keep separate copies of the SlackBuilds in the build
       order, since tagging will have a different meaning and we will add
       another property (mode: install, upgrade, etc.) */

    std::vector<BuildListItem> _builds;

    /* Drawing */
    
    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    InstallBox();
    InstallBox(WINDOW *win, const std::string & name);

    /* Get attributes */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;
    bool installingAllDeps() const;

    /* Creates list based on SlackBuild selected */

    int create(BuildListItem & build,
               std::vector<std::vector<BuildListItem> > & slackbuilds,
               const std::string & action, bool resolve_deps=true,
               bool recheck=false);

    /* User interaction loop. Differs from standard BuildListBox exec() in
       that the space bar is used to tag (select/unselect) items */

    std::string exec();

    /* Make a list of packages from a different repo that will be changed */

    std::vector<const BuildListItem *> checkForeign() const;

    /* Install, upgrade, remove, or reinstall SlackBuild and dependencies */

    int applyChanges(int & nchanged, bool notify_complete=true) const;
};
