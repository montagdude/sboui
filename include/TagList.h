#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "SelectionBox.h"
#include "ListItem.h"
#include "BuildListItem.h"

/*******************************************************************************

Manages tagged SlackBuilds

*******************************************************************************/
class TagList: public SelectionBox {

  private:

    /* List storing all tagged items. The _items list is a subset of this
       showing only what is appropriate for the requested action (install,
       upgrade, etc.). */

    std::vector<ListItem> _tagged;

    /* Drawing */
    
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    TagList();
    TagList(WINDOW *win, const std::string & name);

    /* Edit list (redefined to use _tagged instead of _items) */

    void addItem(ListItem *item);
    void removeItem(ListItem *item);
    void removeItem(unsigned int idx);
    void clearList();

    /* Create _items list to display based on action */

    unsigned int getDisplayList(const std::string & action);

    /* User interaction loop */

    std::string exec();

    /* Returns BuildListItem by index in _items list */

    BuildListItem itemByIdx(unsigned int idx) const;
};
