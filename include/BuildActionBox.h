#pragma once

#include <curses.h>
#include "ListItem.h"
#include "BuildListItem.h"
#include "SelectionBox.h"

/*******************************************************************************

SelectionBox for installing/upgrading/removing/etc. a SlackBuild

*******************************************************************************/
class BuildActionBox: public SelectionBox {
  
  public:

    /* Constructors */

    BuildActionBox();

    /* Destructor */

    ~BuildActionBox();

    /* Sets up actions based on the SlackBuild selected */

    void create(BuildListItem & build, bool limited_actions=false);
};
