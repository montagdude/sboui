#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "ListItem.h"
#include "SelectionBox.h"

/*******************************************************************************

SelectionBox for filters

*******************************************************************************/
class FilterBox: public SelectionBox {
  
  public:

    /* Constructors */

    FilterBox();
    FilterBox(WINDOW *win, const std::string & name);

    /* Destructor */

    ~FilterBox();
};
