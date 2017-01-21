#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "BuildOrderBox.h"

/*******************************************************************************

Displays installed inverse requirements for a SlackBuild

*******************************************************************************/
class InvReqBox: public BuildOrderBox {

  public:

    /* Constructors */

    InvReqBox();
    InvReqBox(WINDOW *win, const std::string & name);

    /* Creates list based on SlackBuild selected */

    void create(BuildListItem & build,
                std::vector<BuildListItem *> & installedlist);
};
