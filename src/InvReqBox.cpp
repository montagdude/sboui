#include <vector>
#include <string>
#include <curses.h>
#include "requirements.h"
#include "BuildListItem.h"
#include "BuildOrderBox.h"
#include "InvReqBox.h"

/*******************************************************************************

Constructors

*******************************************************************************/
InvReqBox::InvReqBox() { _info = "Esc: Back"; }
InvReqBox::InvReqBox(WINDOW *win, const std::string & name)
{
  _info = "Esc: Back";
  _win = win;
  _name = name;
}

/*******************************************************************************

Creates list based on SlackBuild selected. Returns 0 if dependency resolution
succeeded or 1 if some could not be found in the repository.

*******************************************************************************/
void InvReqBox::create(const BuildListItem & build,
                       std::vector<std::vector<BuildListItem> > & slackbuilds)
{
  unsigned int i, nbuilds;
  std::vector<BuildListItem *> invreqlist;

  setName(build.name() + " inverse deps");
  compute_inv_reqs(build, invreqlist, slackbuilds);

  nbuilds = invreqlist.size();
  for ( i = 0; i < nbuilds; i++ ) { addItem(invreqlist[i]); }
}
