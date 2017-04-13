#pragma once

#include <vector>
#include <string>
#include <regex>

/*******************************************************************************

Reads package_blacklist files and checks installed packages for matches

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
};
