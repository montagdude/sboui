#include <string>
#include <curses.h>
#include "TextInput.h"
#include "ToggleInput.h"
#include "SearchBox.h"

/*******************************************************************************

Constructors

*******************************************************************************/
SearchBox::SearchBox()
{
  _caseitem.setName("Case sensitive");
  _caseitem.disable();
  _wholeitem.setName("Whole word");
  _wholeitem.disable();

  addItem(&_entryitem);
  addItem(&_caseitem);
  addItem(&_wholeitem);
}

SearchBox::SearchBox(WINDOW *win, const std::string & msg)
{
  _win = win;
  _msg = msg;

  _caseitem.setName("Case sensitive");
  _caseitem.disable();
  _wholeitem.setName("Whole word");
  _wholeitem.disable();

  addItem(&_entryitem);
  addItem(&_caseitem);
  addItem(&_wholeitem);
}
