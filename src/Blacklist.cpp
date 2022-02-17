#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include "string_util.h"
#include "backend.h"
#include "Blacklist.h"

/*******************************************************************************

Constructor

*******************************************************************************/
Blacklist::Blacklist() { _patterns.resize(0); }

/*******************************************************************************

Reads and stores patterns from blacklist file

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
  unsigned int i, npatterns;

  get_pkg_info(pkg, name, version, arch, build);

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

bool Blacklist::blacklisted(const std::string & pkg, const std::string & name,
                          const std::string & version, const std::string & arch,
                          const std::string & build) const
{
  unsigned int i, npatterns;

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

/*******************************************************************************

Checks for match by name only (used for not-installed SlackBuilds)

*******************************************************************************/
bool Blacklist::nameBlacklisted(const std::string & name) const
{
  unsigned int i, npatterns;

  npatterns = _patterns.size();
  for ( i = 0; i < npatterns; i++ )
  {
    if (std::regex_match(name, _patterns[i])) { return true; }
  }

  return false;
}
