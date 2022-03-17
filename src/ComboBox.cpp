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
#include "MouseEvent.h"

/*******************************************************************************

Places and sizes list box

*******************************************************************************/
void ComboBox::placeListBox(int y_offset)
{
  int width, height, top, left, begy, begx, rows, cols;

  // Get position and dimensions of window - don't let the list extend beyond

  getbegyx(_win, begy, begx);
  getmaxyx(_win, rows, cols);

  // First see if the box can scroll out below

  _list.preferredSize(height, width);

  left = posx() + begx;
  top = posy()-y_offset + begy;
  if (posy()-y_offset + (height-1) <= rows-1) { top = posy()-y_offset + begy; }
  else if (posy()-y_offset - (height-1) >= 0)
    top = posy()-y_offset + begy - (height-1);
  else
  {
    top = posy()-y_offset + begy;
    height = rows - (posy()-y_offset);
  }
  mvwin(_listwin, top, left);
  wresize(_listwin, height, width);
}

/*******************************************************************************

User interaction with list

*******************************************************************************/
std::string ComboBox::execList(int y_offset, MouseEvent * mevent)
{
  int rows, cols, left, top;
  std::string retval;

  placeListBox(y_offset);
  retval = _list.exec(mevent);

  getmaxyx(_win, rows, cols);
  left = std::floor(double(cols)/2.);
  top = std::floor(double(rows)/2.);
  mvwin(_listwin, top, left);
  wresize(_listwin, 0, 0);
  wrefresh(_listwin);
  _parent->draw(true);

  return retval;
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
  // Calling placeListBox here ensures list window is proper dimensions when
  // _firstprint is calculated

  placeListBox(0);
  _list.setHighlight(idx); 
}
void ComboBox::setChoice(const std::string & choice)
{
  placeListBox(0);
  _list.setHighlight(choice); 
}

/*******************************************************************************

Handles mouse event

*******************************************************************************/
std::string ComboBox::handleMouseEvent(MouseEvent * mevent, int y_offset)
{
  int begy, begx, ycurs, xcurs;

  getbegyx(_win, begy, begx);
  ycurs = mevent->y() - begy;
  xcurs = mevent->x() - begx;

  if ( (mevent->button() == 1) || (mevent->button() == 3) )
  {
    // Check for clicking in the ComboBox

    if ( (xcurs >= _posx) && (xcurs < _posx+_width) &&
         (ycurs == _posy-y_offset) )
    {
      if (execList(y_offset, mevent) == signals::mouseEvent)
        return signals::mouseEvent;
      else
        return signals::nullEvent;    // Because the event was handled here
    }
  }

  return signals::mouseEvent;         // Defer to InputBox to handle event
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
    if (colors.turnOn(_win, "fg_highlight_active", "bg_highlight_active") != 0)
      wattron(_win, A_REVERSE);
  }
  else { colors.turnOn(_win, "fg_combobox", "bg_combobox"); }

  // Print selection and indicator

  wmove(_win, _posy-y_offset, _posx);
  wprintw(_win, "%s", _list.highlightedItem()->name().c_str());
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
std::string ComboBox::exec(int y_offset, MouseEvent * mevent)
{
  int ch;
  bool getting_input;
  std::string retval;
  MEVENT event;

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
        execList(y_offset, mevent);
        retval = signals::keySpace;
        getting_input = false;
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
      case KEY_PPAGE:
        retval = signals::highlightPrevPage;
        getting_input = false;
        break;
      case KEY_NPAGE:
        retval = signals::highlightNextPage;
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
      case KEY_RIGHT:
        retval = signals::keyRight;
        getting_input = false;
        break;
      case KEY_LEFT:
        retval = signals::keyLeft;
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

      // Mouse

      case KEY_MOUSE:
        if ( (getmouse(&event) == OK) && mevent )
        {
          mevent->recordClick(event);
          retval = handleMouseEvent(mevent, y_offset);
          getting_input = false;
        }
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
std::string ComboBox::choice() const { return _list.highlightedName(); }
