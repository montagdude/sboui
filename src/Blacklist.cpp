#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include "string_util.h"
#include "Blacklist.h"

/*******************************************************************************

Constructor

*******************************************************************************/
Blacklist::Blacklist() { _patterns.resize(0); }

/*******************************************************************************

Reads and stores patterns from package_blacklist file

*******************************************************************************/
int Blacklist::read(const std::string & filename)
{
  std::ifstream file;
  std::string line;

  file.open(filename.c_str());
  if (not file.is_open()) { return 1; }

  while (! file.eof())
  {
    std::getline(file, line); 
    line = remove_leading_whitespace(line);
    if (line[0] == '#') { continue; }
    line = remove_comment(line, '#');
    line = trim(line);
    if (line.size() > 0)
    {
      std::regex reg(line);
      _patterns.push_back(reg);
    }
  }

  file.close();

  return 0;
}

/*******************************************************************************

Checks installed package for matches in name, version, arch, build, and fullname

*******************************************************************************/
bool Blacklist::blacklisted(const std::string & pkg) const
{
  std::string name, version, arch, build;
  std::vector<std::string> splitpkg;
  unsigned int i, nsplit, npatterns;

  // Get package info (could put this in backend.cpp, but this is the only thing
  // that needs to know all the components of the package name)

  splitpkg = split(pkg, '-');
  nsplit = splitpkg.size();
  name = "";
  for ( i = 0; i < nsplit-4; i++ ) { name += splitpkg[i] + "-"; }
  name += splitpkg[nsplit-4];
  version = splitpkg[nsplit-3];
  arch = splitpkg[nsplit-2];
  build = splitpkg[nsplit-1];

  // Check for blacklist pattern matching info in package name

  npatterns = _patterns.size();
  for ( i = 0; i < npatterns; i++ )
  {
    if (std::regex_match(name, _patterns[i])) { return true; }
    if (std::regex_match(version, _patterns[i])) { return true; }
    if (std::regex_match(arch, _patterns[i])) { return true; }
    if (std::regex_match(build, _patterns[i])) { return true; }
    if (std::regex_match(pkg, _patterns[i])) { return true; }
  }

  return false;
}
