#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "BuildListItem.h"
#include "TextInput.h"
#include "BuildOptionsBox.h"

/*******************************************************************************

Constructor

*******************************************************************************/
BuildOptionsBox::BuildOptionsBox()
{
  addItem(&_entryitem);
  _entryitem.setWidth(30);      // Gets resized later in draw()
  _entryitem.setPosition(3,1);
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void BuildOptionsBox::setBuild(const BuildListItem & build)
{
  _msg = build.name() + " build options";
  _entryitem.setText(build.getProp("build_options"));
}

/*******************************************************************************

Get attributes

*******************************************************************************/
std::string BuildOptionsBox::entry() const { return _entryitem.text(); }

/*******************************************************************************

Redraws box and entry

*******************************************************************************/
void BuildOptionsBox::draw(bool force)
{
  int rows, cols;

  // Set width of text box

  getmaxyx(_win, rows, cols);
  _entryitem.setWidth(cols-2);

  if (force) { _redraw_type = "all"; }

  if (_redraw_type == "all") 
  { 
    wclear(_win);
    colors.setBackground(_win, "fg_popup", "bg_popup");
    redrawFrame();
    redrawScrollIndicator();
    redrawAllItems(force);
  }
  else
  {
    redrawScrollIndicator();
    redrawChangedItems(force);
  }
  wrefresh(_win);
}
