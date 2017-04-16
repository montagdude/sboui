#pragma once

#include <string>
#include <vector>
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

    void readInstalledProps(std::vector<std::string> & installedpkgs);
    int readPropsFromRepo();

    // Checks whether a SlackBuild can be upgraded

    bool upgradable() const;
};
