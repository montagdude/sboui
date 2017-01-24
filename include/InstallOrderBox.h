#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "BuildOrderBox.h"

/*******************************************************************************

Displays build order for installs/upgrades and allows user to select which to
install/upgrade

*******************************************************************************/
class InstallOrderBox: public BuildOrderBox {

  private:

    /* For this class, keep separate copies of the SlackBuilds in the build
       order, since tagging will have a different meaning and we will add
       another property (mode: install or upgrade) */

    std::vector<BuildListItem> _builds;

    /* Drawing */
    
    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    InstallOrderBox();
    InstallOrderBox(WINDOW *win, const std::string & name);

    /* Get attributes */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;

    /* Creates list based on SlackBuild selected */

    int create(BuildListItem & build,
               std::vector<BuildListItem> & slackbuilds);

    /* User interaction loop. Differs from standard BuildListBox exec() in
       that the space bar is used to tag (select/unselect) items */

    std::string exec();

    /* Install or upgrade SlackBuild and dependencies */

    int applyChanges() const;
};
