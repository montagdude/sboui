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
       order, because it is easier to work with BuildListItems than ListItems
       for many of the operations. */

    std::vector<BuildListItem *> _builds;
    int _ndeps, _ninvdeps;

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

    int numDeps() const;    // See notes in InstallBox.cpp on these two methods
    int numInvDeps() const; // ------------------------------------------------

    bool installingAllDeps() const;
    bool installingRequested() const;

    /* Creates list based on SlackBuild selected */

    int create(BuildListItem & build,
               std::vector<std::vector<BuildListItem> > & slackbuilds,
               const std::string & action, bool resolve_deps=true,
               bool batch=false, bool rebuild_inv_deps=false);

    /* User interaction loop. Differs from standard BuildListBox exec() in
       that the space bar is used to tag (select/unselect) items */

    std::string exec();

    /* Make a list of packages from a different repo that will be changed */

    std::vector<const BuildListItem *> checkForeign() const;

    /* Install, upgrade, remove, or reinstall SlackBuild and dependencies */

    int applyChanges(int & ninstalled, int & nupgraded, int & nreinstalled,
                     int & nremoved);
};
