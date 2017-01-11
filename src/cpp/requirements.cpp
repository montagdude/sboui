#include <string>
#include <vector>
#include <algorithm>       // reverse
#include "BuildListItem.h"
#include "backend.h"       // get_reqs, split
#include "requirements.h"

/*******************************************************************************

Returns pointer to correct entry in _slackbuilds vector from given name. Returns
null pointer if not found.

*******************************************************************************/
BuildListItem * build_from_name(const std::string & name,
                                std::vector<BuildListItem> & slackbuilds)
{
  unsigned int i, nbuilds;

  nbuilds = slackbuilds.size();
  for ( i = 0; i < nbuilds; i++ )
  {
    if (slackbuilds[i].name() == name) { return &slackbuilds[i]; }
  }

  return NULL;
}

/*******************************************************************************

Adds required SlackBuild to dependency list, removing any instance already
present in the list

*******************************************************************************/
void add_req(BuildListItem *build,
             std::vector<BuildListItem *> & reqlist)
{
  unsigned int i, nreqs;

  nreqs = reqlist.size();
  for ( i = 0; i < nreqs; i++ )
  {
    if (reqlist[i]->name() == build->name()) 
    { 
      reqlist.erase(reqlist.begin()+i); 
      break;
    }
  }
  reqlist.push_back(build);
}

/*******************************************************************************

Recursively computes list of requirements for a SlackBuild. List must be
reversed after calling this to get the proper build order. Returns 1 if a
requirement is not found in the list.

*******************************************************************************/
int get_reqs_recursive(const BuildListItem & build,
                       std::vector<BuildListItem *> & reqlist,
                       std::vector<BuildListItem> & slackbuilds)
{
  unsigned int i, ndeps;
  std::vector<std::string> deplist;
  BuildListItem *newbuild;

  if (build.getBoolProp("installed")) { deplist = 
                                        split(build.getProp("requires")); }
  else { deplist = split(get_reqs(build)); }
  
  ndeps = deplist.size();
  for ( i = 0; i < ndeps; i++ )
  { 
    if (deplist[i] != "%README%")
    { 
      newbuild = build_from_name(deplist[i], slackbuilds);
      if (newbuild) { add_req(newbuild, reqlist); }
      else { return 1; }
      get_reqs_recursive(*newbuild, reqlist, slackbuilds); 
    }
  }

  return 0;
}

/*******************************************************************************

Computes list of requirements needed for a SlackBuild in the correct build
order. Note: slackbuilds vector is not passed as const, because reqlist entries
must be non-const for later use in BuildListBox.

*******************************************************************************/
int compute_reqs_order(const BuildListItem & build,
                       std::vector<BuildListItem *> & reqlist,
                       std::vector<BuildListItem> & slackbuilds)
{
  int check;

  reqlist.resize(0);
  check = get_reqs_recursive(build, reqlist, slackbuilds);
  if (check == 0) { std::reverse(reqlist.begin(), reqlist.end()); }

  return check;
}  
