#pragma once

#include <string>
#include "ListItem.h"

/*******************************************************************************

List item for Help window

*******************************************************************************/
class HelpItem: public ListItem {

  public:

    // Constructors

    HelpItem();
    HelpItem(const std::string & name, const std::string & shortcut,
             bool header=false, bool space=false);
};
