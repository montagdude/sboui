#include <vector>
#include <curses.h>
#include "BuildListItem.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListBox.h"

/*******************************************************************************

Filters lists by all SlackBuilds

*******************************************************************************/
void filter_all(std::vector<BuildListItem> & slackbuilds,
                std::vector<CategoryListItem> & categories,
                WINDOW *blistboxwin, CategoryListBox & clistbox,
                std::vector<BuildListBox> & blistboxes)
{
  unsigned int i, j, nbuilds, ncategories;
  BuildListBox initlistbox;

  nbuilds = slackbuilds.size();
  ncategories = categories.size();

  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  for ( j = 0; j < ncategories; j++ )
  {
    clistbox.addItem(&categories[j]);
    BuildListBox blistbox;
    blistbox.setWindow(blistboxwin);
    blistbox.setName(categories[j].name());
    blistbox.setActivated(false); 
    blistboxes.push_back(blistbox);
  }
  for ( i = 0; i < nbuilds; i++ )
  {
    for ( j = 0; j < ncategories; j++ )
    {
      if (slackbuilds[i].getProp("category") == categories[j].name())
        blistboxes[j].addItem(&slackbuilds[i]);
    }
  }

  // Check wheteher categories should be tagged

  for ( j = 0; j < ncategories; j++ )
  {
    if (blistboxes[j].allTagged())
      clistbox.itemByIdx(j)->setBoolProp("tagged", true);
    else
      clistbox.itemByIdx(j)->setBoolProp("tagged", false);
  }

  // Initialize with empty lists if repo is empty

  if (nbuilds == 0)
  {
    initlistbox.setWindow(blistboxwin);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    blistboxes.push_back(initlistbox);
  }
} 
