#pragma once

#include <string>
#include <curses.h>
#include "TextInput.h"
#include "InputBox.h"

/*******************************************************************************

Search dialog

*******************************************************************************/
class SearchBox: public InputBox {

  private:

    TextInput _entryitem;

  public:

    /* Constructors */

    SearchBox();
    SearchBox(WINDOW *win, const std::string & msg);
};
