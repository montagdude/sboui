#pragma once

#include <vector>
#include <string>

/*******************************************************************************

Reads ignore_versions file and checks SlackBuilds and versions for matches

*******************************************************************************/
class IgnoreVersions {

  private:

    std::vector<std::string> _names;
    std::vector<std::string> _versions;

  public:

    /* Constructor */

    IgnoreVersions();

    /* Reads ignore_versions file */

    void read(const std::string & filename); 

    /* Checks SlackBuild for matching version that was listed in the file */
    bool ignoreVersion(const std::string & name,
                       const::std::string & version) const;
};
