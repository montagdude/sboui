#pragma once

#include <string>
#include <vector>
#include "ListItem.h"

/*******************************************************************************

List item that describes a SlackBuild

*******************************************************************************/
class BuildListItem: public ListItem {

  private:

    // Checks whether a SlackBuild can be upgraded

    bool differsByKernel(const std::string & installed_version,
                         const std::string & available_version) const;
    bool upgradable() const;

  public:

    // Constructor

    BuildListItem();

    // Convert ListItem to BuildListItem

    void operator = (const ListItem & item);

    // Reads properties from repo 

    void readInstalledProps(std::vector<std::string> & installedpkgs);
    int readPropsFromRepo();

    // Determines BUILD number from last portion of package name

    void parseBuildNum(std::string & build);

    // Build options as string of environment variables

    std::string buildOptionsEnv() const;
};
