#include <string>
#include <vector>
#include <algorithm>       // reverse
#include "BuildListItem.h"
#include "backend.h"       // get_reqs
#include "string_util.h"   // split
#include "requirements.h"

/*******************************************************************************

Gets indices of correct entry in _slackbuilds vector from given name. Returns
1 if not found, 0 if found.

*******************************************************************************/
int build_from_name(const std::string & name,
                    std::vector<std::vector<BuildListItem> > & slackbuilds,
                    int & idx0, int & idx1)
{
  int i, j, nbuilds, ncategories;

  ncategories = slackbuilds.size();
  for ( i = 0; i < ncategories; i++ )
  {
    nbuilds = slackbuilds[i].size();
    for ( j = 0; j < nbuilds; j++ )
    { 
      if (slackbuilds[i][j].name() == name) 
      { 
        idx0 = i;
        idx1 = j;
        return 0;
      }
    }
  }

  return 1;
}

/*******************************************************************************

Returns index of correct entry in installedlist vector from given name. Returns
-1 if not found.

*******************************************************************************/
int build_from_name(const std::string & name,
                    std::vector<BuildListItem *> & installedlist)
{
  int idx, i, ninstalled;

  idx = -1;
  ninstalled = installedlist.size();
  for ( i = 0; i < ninstalled; i++ )
  {
    if (installedlist[i]->name() == name) 
    { 
      idx = i;
      break;
    }
  }

  return idx;
}

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
      check = build_from_name(deplist[i], slackbuilds, idx0, idx1);
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
