#include <string>
#include <vector>
#include <fstream>
#include "string_util.h"
#include "IgnoreVersions.h"

/*******************************************************************************

Constructor

*******************************************************************************/
IgnoreVersions::IgnoreVersions()
{
  _names.resize(0);
  _versions.resize(0);
}

/*******************************************************************************

Reads and stores SlackBuild:version pairs from file

*******************************************************************************/
void IgnoreVersions::read(const std::string & filename)
{
  std::ifstream file;
  std::string line;
  std::vector<std::string> splitline;

  file.open(filename.c_str());
  if (not file.is_open()) { return; }

  while (! file.eof())
  {
    std::getline(file, line); 
    line = remove_leading_whitespace(line);
    if (line[0] == '#') { continue; }
    line = remove_comment(line, '#');
    line = trim(line);
    splitline = split(line, ':');
    if (splitline.size() < 2) { continue; }
    _names.push_back(trim(splitline[0]));
    _versions.push_back(trim(splitline[1]));
  }
  file.close();

  return;
}

/*******************************************************************************

Checks for name:version match in list

*******************************************************************************/
bool IgnoreVersions::ignoreVersion(const std::string & name,
                                   const std::string & version) const
{
  bool ignore = false;
  unsigned int i, nnames;

  nnames = _names.size();
  for ( i = 0; i < nnames; i++ )
  {
    if ( (_names[i] == name) && (_versions[i] == version) )
    {
      ignore = true;
      break;
    }
  }

  return ignore;
}
