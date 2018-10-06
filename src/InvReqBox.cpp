#include <vector>
#include <string>
#include <curses.h>
#include "requirements.h"
#include "signals.h"
#include "BuildListItem.h"
#include "BuildOrderBox.h"
#include "InvReqBox.h"

/*******************************************************************************

Constructors

*******************************************************************************/
InvReqBox::InvReqBox()
{ 
  std::vector<std::string> buttons(1), button_signals(1);

  buttons[0] = "  Back  ";
  button_signals[0] = signals::quit;
  setButtons(buttons, button_signals);
}

InvReqBox::InvReqBox(WINDOW *win, const std::string & name)
{
  std::vector<std::string> buttons(1), button_signals(1);

  buttons[0] = "  Back  ";
  button_signals[0] = signals::quit;
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
