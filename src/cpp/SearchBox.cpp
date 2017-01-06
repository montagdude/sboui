#include <string>
#include <curses.h>
#include "TextInput.h"
#include "SearchBox.h"

/*******************************************************************************

Constructors

*******************************************************************************/
SearchBox::SearchBox()
{
  addItem(&_entryitem);
}

SearchBox::SearchBox(WINDOW *win, const std::string & msg)
{
  _win = win;
  _msg = msg;
  addItem(&_entryitem);
}
