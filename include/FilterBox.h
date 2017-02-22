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

    /* Constructors and destructor */

    FilterBox();
    FilterBox(WINDOW *win, const std::string & name);
    ~FilterBox();
};
