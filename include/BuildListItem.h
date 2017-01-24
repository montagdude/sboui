#pragma once

#include <string>
#include "ListItem.h"

/*******************************************************************************

List item that describes a SlackBuild

*******************************************************************************/
class BuildListItem: public ListItem {

  public:

    // Constructor

    BuildListItem();

    // Checks whether a SlackBuild can be upgraded

    bool upgradable() const;
};
