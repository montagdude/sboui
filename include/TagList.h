#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "SelectionBox.h"
#include "ListItem.h"
#include "BuildListItem.h"
#include "MouseEvent.h"

/*******************************************************************************

Manages tagged SlackBuilds

*******************************************************************************/
class TagList: public SelectionBox {

  private:

    /* List storing all tagged items. The _items list is a subset of this
       showing only what is appropriate for the requested action (install,
       upgrade, etc.). */

    std::vector<ListItem *> _tagged;

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

    /* Get properties */

    unsigned int numTagged() const;

    /* Create _items list to display based on action */

    unsigned int getDisplayList(const std::string & action);

    /* Handles mouse event */

    std::string handleMouseEvent(MouseEvent * mevent);

    /* User interaction loop */

    std::string exec(MouseEvent * mevent=NULL);

    /* Returns pointer to item by index in _tagged list */

    ListItem * taggedByIdx(unsigned int idx);
};
