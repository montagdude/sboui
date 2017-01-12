#pragma once

#include <vector>
#include <curses.h>
#include "ListItem.h"
#include "BuildListItem.h"
#include "SelectionBox.h"

/*******************************************************************************

SelectionBox for installing/upgrading/removing/etc. a SlackBuild

*******************************************************************************/
class BuildActionBox: public SelectionBox {
  
  private:

    std::vector<ListItem> _choices;

  public:

    /* Constructors */

    BuildActionBox();
    BuildActionBox(WINDOW *win, const std::string & name);
    BuildActionBox(WINDOW *win, const std::string & name,
                   const BuildListItem & build);

    /* Sets up actions based on the SlackBuild selected */

    void create(const BuildListItem & build);
};
