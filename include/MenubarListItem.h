#pragma once

#include <string>
#include "ListItem.h"

/*******************************************************************************

List item for menubar

*******************************************************************************/
class MenubarListItem: public ListItem {

  public:

    // Constructors

    MenubarListItem();
    MenubarListItem(const std::string & name, const std::string & shortcut,
                    int hotkey);
};
