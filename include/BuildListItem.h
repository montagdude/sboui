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

    // Convert ListItem to BuildListItem

    void operator = (const ListItem & item);

    // Reads properties from repo 

    void readInstalledProps();
    void readPropsFromRepo();

    // Checks whether a SlackBuild can be upgraded

    bool upgradable() const;
};
