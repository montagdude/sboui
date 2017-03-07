#include <string>
#include <cmath>     // floor
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "ListItem.h"
#include "InputItem.h"
#include "InputBox.h"
#include "ComboBox.h"

using namespace color_settings;

/*******************************************************************************

Places and sizes list box

*******************************************************************************/
void ComboBox::placeListBox()
{
  int width, height, top, left, begy, begx, rows, cols;

  // Get position and dimensions of window - don't let the list extend beyond

  getbegyx(_win, begy, begx);
  getmaxyx(_win, rows, cols);

  // First see if the box can scroll out below

  _list.preferredSize(height, width);

  left = posx() + begx;
  top = posy() + begy;
  if (posy() + (height-1) <= rows-1) { top = posy() + begy; }
  else if (posy() - (height-1) >= 0) { top = posy() + begy - (height-1); }
  else
  {
    top = posy() + begy;
    height = rows - posy();
  }
  mvwin(_listwin, top, left);
  wresize(_listwin, height, width);
}

/*******************************************************************************

User interaction with list

*******************************************************************************/
void ComboBox::execList()
{
  int rows, cols, left, top;

  placeListBox();
  _list.exec();

  getmaxyx(_win, rows, cols);
  left = std::floor(double(cols)/2.);
  top = std::floor(double(rows)/2.);
  mvwin(_listwin, top, left);
  wresize(_listwin, 0, 0);
  wrefresh(_listwin);
  _parent->draw(true);
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
ComboBox::ComboBox()
{ 
  _parent = NULL;
  _redraw_type = "box"; _item_type = "ComboBox";
  _selectable = true;
  _listwin = newwin(1, 1, 0, 0); 
  _list.setWindow(_listwin);
}

ComboBox::ComboBox(InputBox *parent)
{ 
  _parent = parent;
  _redraw_type = "box"; _item_type = "ComboBox";
  _selectable = true;
  _listwin = newwin(1, 1, 0, 0); 
  _list.setWindow(_listwin);
}

ComboBox::~ComboBox() 
{ 
  _list.clearList();
  delwin(_listwin); 
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void ComboBox::setParent(InputBox *parent) { _parent = parent; }
void ComboBox::addChoice(const std::string & choice)
{
  _list.addItem(new ListItem(choice));
  if (_width < int(choice.size())+4) { _width = choice.size()+4; }
}

void ComboBox::setChoice(unsigned int idx)
{ 
  placeListBox();
  _list.setHighlight(idx); 
}

void ComboBox::setChoice(const std::string & choice)
{ 
  placeListBox();
  _list.setHighlight(choice); 
}

/*******************************************************************************

Draws ComboBox

*******************************************************************************/
void ComboBox::draw(int y_offset, bool force, bool highlight)
{
  int nspaces;

  // Turn on color

  if (highlight)
  {
    if (colors.turnOn(_win, fg_highlight_active, bg_highlight_active) != 0)
      wattron(_win, A_REVERSE);
  }
  else { colors.turnOn(_win, fg_combobox, bg_combobox); }

  // Print selection and indicator

  wmove(_win, _posy-y_offset, _posx);
  wprintw(_win, _list.highlightedItem()->name().c_str());
  nspaces = _width - _list.highlightedItem()->name().size() - 3;
  printSpaces(nspaces);
  wprintw(_win, "[^]");

  if (colors.turnOff(_win) != 0)
  {
    if (highlight) { wattroff(_win, A_REVERSE); }
  }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke

*******************************************************************************/
std::string ComboBox::exec(int y_offset)
{
  int ch;
  bool getting_input;
  std::string retval;

  const int MY_ESC = 27;
  const int MY_TAB = 9;
  const int MY_SHIFT_TAB = 353;

  getting_input = true;
  while (getting_input)
  {
    // Redraw

    draw(y_offset, false, true);
    
    // Get user input

    switch (ch = getch()) {

      // Enter key: return Enter signal

      case '\n':
      case '\r':
      case KEY_ENTER:
        retval = signals::keyEnter;
        getting_input = false;
        break;

      // Space: display list box

      case ' ':
        execList();
        break;

      // Navigation keys

      case KEY_HOME:
        retval = signals::highlightFirst;
        getting_input = false;
        break;
      case KEY_END:
        retval = signals::highlightLast;
        getting_input = false;
        break;
      case KEY_UP:
      case MY_SHIFT_TAB:
        retval = signals::highlightPrev;
        getting_input = false;
        break;
      case KEY_DOWN:
      case MY_TAB:
        retval = signals::highlightNext;
        getting_input = false;
        break;

      // Resize signal

      case KEY_RESIZE:
        retval = signals::resize;
        getting_input = false;
        break;
      
      // Quit key

      case MY_ESC:
        retval = signals::quit;
        getting_input = false;
        break;
 
      default:
        retval = char(ch);
        getting_input = false;
        break;
    }
  }
  return retval;
}

/*******************************************************************************

Accessing properties

*******************************************************************************/
int ComboBox::getIntProp() const { return _list.highlight(); }
std::string ComboBox::getStringProp() const { return _list.highlightedName(); }
