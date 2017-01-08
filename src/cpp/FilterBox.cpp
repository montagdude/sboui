#include <string>
#include <curses.h>
#include "Color.h"
#include "color_settings.h"
#include "ListItem.h"
#include "SelectionBox.h"
#include "FilterBox.h"

using namespace color;

/*******************************************************************************

Constructors

*******************************************************************************/
FilterBox::FilterBox()
{
  unsigned int i;

  _name = "Select a filter";
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;

  _choices.resize(4);
  _choices[0].setName("All");
  _choices[1].setName("Installed");
  _choices[2].setName("Upgradable");
  _choices[3].setName("Non-dependencies");
  for ( i = 0; i < 4; i++ ) { addItem(&_choices[i]); }
}

FilterBox::FilterBox(WINDOW *win, const std::string & name)
{
  unsigned int i;

  _win = win;
  _name = name;
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;

  _choices.resize(4);
  _choices[0].setName("All");
  _choices[1].setName("Installed");
  _choices[2].setName("Upgradable");
  _choices[3].setName("Non-dependencies");
  for ( i = 0; i < 4; i++ ) { addItem(&_choices[i]); }
}
