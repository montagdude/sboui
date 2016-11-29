#include <string>
#include <curses.h>
#include <cmath>    // floor
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

  _win = NULL;
  _name = "Select a filter";
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
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
  _redraw_type = "all";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 6;

  _choices.resize(4);
  _choices[0].setName("All");
  _choices[1].setName("Installed");
  _choices[2].setName("Upgradable");
  _choices[3].setName("Non-dependencies");
  for ( i = 0; i < 4; i++ ) { addItem(&_choices[i]); }
}

/*******************************************************************************

Draws filter box (frame, items, etc.) as needed

*******************************************************************************/
void FilterBox::draw(bool force)
{
  int pair_popup;

  // Draw list elements

  if ( (_redraw_type == "all") || force )
  {
    wclear(_win);
    pair_popup = colors.pair(fg_popup, bg_popup);
    if (pair_popup != -1) { wbkgd(_win, COLOR_PAIR(pair_popup)); }
  }
  if ( (_redraw_type != "none") || force ) { redrawFrame(); }
  if ( (_redraw_type == "all") || (_redraw_type == "items") || force ) {
                                                            redrawAllItems(); }
  else if (_redraw_type == "changed") { redrawChangedItems(); }
  wrefresh(_win);
}
