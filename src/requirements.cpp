#include <string>
#include <vector>
#include <algorithm>       // reverse
#include "BuildListItem.h"
#include "backend.h"       // get_reqs, find_slackbuild, find_build_in_list
#include "string_util.h"   // split
#include "requirements.h"

/*******************************************************************************

Adds required SlackBuild to dependency list, removing any instance already
present in the list

*******************************************************************************/
void add_req(BuildListItem * build,
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
                       std::vector<std::vector<BuildListItem> > & slackbuilds)
{
  unsigned int i, ndeps;
  std::vector<std::string> deplist;
  int idx0, idx1, check;

  if (build.getBoolProp("installed")) { deplist = 
                                        split(build.getProp("requires")); }
  else { deplist = split(get_reqs(build)); }
  
  check = 0;
  ndeps = deplist.size();
  for ( i = 0; i < ndeps; i++ )
  { 
    if (deplist[i] != "%README%")
    { 
      check = find_slackbuild(deplist[i], slackbuilds, idx0, idx1);
      if (check == 0) { add_req(&slackbuilds[idx0][idx1], reqlist); }
      else { return 1; }
      check = get_reqs_recursive(slackbuilds[idx0][idx1], reqlist, slackbuilds); 
      if (check != 0) { return check; }
    }
  }

  return check;
}

/*******************************************************************************

Computes list of requirements needed for a SlackBuild in the correct build
order. 

*******************************************************************************/
int compute_reqs_order(const BuildListItem & build,
                       std::vector<BuildListItem *> & reqlist,
                       std::vector<std::vector<BuildListItem> > & slackbuilds)
{
  int check;

  reqlist.resize(0);
  check = get_reqs_recursive(build, reqlist, slackbuilds);
  if (check == 0) { std::reverse(reqlist.begin(), reqlist.end()); }

  return check;
}  

/*******************************************************************************

Recursively finds installed SlackBuilds that depend on a given SlackBuild

*******************************************************************************/
void get_inverse_reqs_recursive(const BuildListItem & build,
                      std::vector<BuildListItem *> & invreqlist,
                      std::vector<BuildListItem *> & installedlist)
{
  unsigned int i, j, ninstalled, ndeps;
  std::vector<std::string> deplist;

  ninstalled = installedlist.size();
  for ( i = 0; i < ninstalled; i++ )
  {
    deplist = split(installedlist[i]->getProp("requires"));
    ndeps = deplist.size();
    for ( j = 0; j < ndeps; j++ )
    {
      if (deplist[j] == build.name())
      {
        add_req(installedlist[i], invreqlist);
        get_inverse_reqs_recursive(*installedlist[i], invreqlist, 
                                   installedlist);
        break;
      }
    }
  }
}        

/*******************************************************************************

Computes list of installed SlackBuilds that depend on a given SlackBuild

*******************************************************************************/
void compute_inv_reqs(const BuildListItem & build,
                      std::vector<BuildListItem *> & invreqlist,
                      std::vector<BuildListItem *> & installedlist)
{
  invreqlist.resize(0);
  get_inverse_reqs_recursive(build, invreqlist, installedlist);
}
