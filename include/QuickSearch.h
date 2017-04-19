#pragma once

#include <string>
#include <curses.h>
#include "TextInput.h"

/*******************************************************************************

String input item used for quick search

*******************************************************************************/
class QuickSearch: public TextInput {

  public:

    /* User interaction */

    virtual std::string exec(int y_offset);
};
