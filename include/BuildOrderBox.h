#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "MouseEvent.h"

/*******************************************************************************

Displays build order

*******************************************************************************/
class BuildOrderBox: public BuildListBox {

  protected:

    /* Drawing */
    
    virtual void redrawFrame();
    virtual void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    BuildOrderBox();
    BuildOrderBox(WINDOW *win, const std::string & name);

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;

    /* Creates list based on SlackBuild selected */

    int create(BuildListItem & build,
               std::vector<std::vector<BuildListItem> > & slackbuilds,
               const std::string & mode="forward");

    /* Handles mouse event */

    std::string handleMouseEvent(MouseEvent * mevent);
};
