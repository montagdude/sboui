#pragma once

#include <string>
#include "ListItem.h"

/*******************************************************************************

List item that describes a SlackBuild. Populates most of the different ListBoxes
in sboui.

*******************************************************************************/
class SlackBuildListItem: public ListItem {

  private:

    std::string _category, _installed_version, _available_version;
    bool _tagged, _installed;

  public:

    // Constructor

    SlackBuildListItem();

    // Set properties

    void setCategory(const std::string & category);
    void setInstalledVersion(const std::string & installed_version);
    void setAvailableVersion(const std::string & available_version);
    void setTagged(bool tagged);
    void setInstalled(bool installed);

    // Get properties

    const std::string & category() const;
    const std::string & installedVersion() const;
    const std::string & availableVersion() const;
    bool tagged() const;
    bool installed() const;
};
