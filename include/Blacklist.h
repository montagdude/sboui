#pragma once

#include <vector>
#include <string>
#include <regex>

/*******************************************************************************

Reads blacklist files and checks packages and SlackBuilds for matches

*******************************************************************************/
class Blacklist {

  private:

    std::vector<std::regex> _patterns;

  public:

    /* Constructor */

    Blacklist();

    /* Reads blacklist patterns from file */

    int read(const std::string & filename); 

    /* Checks installed package for matches in name, version, arch, build, and
       fullname */

    bool blacklisted(const std::string & pkg) const;
    bool blacklisted(const std::string & pkg, const std::string & name,
                     const std::string & version, const std::string & arch,
                     const std::string & build) const;

    /* Checks not-installed package for match in name only */

    bool nameBlacklisted(const std::string & name) const;
};
