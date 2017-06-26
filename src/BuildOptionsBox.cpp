#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "string_util.h"
#include "BuildListItem.h"
#include "TextInput.h"
#include "Label.h"
#include "BuildOptionsBox.h"

/*******************************************************************************

Sets up input box

*******************************************************************************/
void BuildOptionsBox::setUp()
{
  unsigned int i, nentries;

  clear();
  addItem(&_addlbl);
  nentries = _entries.size();
  for ( i = 0; i < nentries; i++ )
  {
    addItem(&_entries[i]);
    _entries[i].setWidth(30);      // Gets resized later in draw()
    _entries[i].setPosition(3+2*(i+1),1);
    _entries[i].setLabel(int_to_string(i+1) + ". ");
  }
  if (nentries > 0)
  {
    addItem(&_removelbl);
    _removelbl.setPosition(3+2*(nentries+1),1);
    for ( i = 0; i < nentries; i++ )
    {
      addItem(new Label(false, true));
      _items[numItems()-1]->setPosition(3+2*(i+1)-1,1);
      _items[numItems()-1]->setWidth(30); // Gets resized later in draw()
    }
    addItem(new Label(false, true));
    _items[numItems()-1]->setPosition(3+2*(nentries+1)-1,1);
    _items[numItems()-1]->setWidth(30); // Gets resized later in draw()
  }
}

/*******************************************************************************

Add or remove an entry

*******************************************************************************/
void BuildOptionsBox::addEntry()
{
  TextInput entry;
  _entries.push_back(entry);
  setUp();
}

void BuildOptionsBox::removeLast()
{
  _entries.erase(_entries.begin() + numEntries() - 1);
  setUp();
  if (numEntries() > 0) { highlightLast(); }
  else { highlightFirst(); }
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
BuildOptionsBox::BuildOptionsBox()
{
  TextInput first_entry;

  _addlbl.setSelectable(true);
  _addlbl.setName("+ Add (press space)");
  _addlbl.setPosition(3,1);
  _addlbl.setColor(colors.getPair("header_popup", "bg_popup"));
  _removelbl.setSelectable(true);
  _removelbl.setName("- Remove last (press space)");
  _removelbl.setColor(colors.getPair("header_popup", "bg_popup"));
  _entries.clear();
  _entries.push_back(first_entry);
  setUp();
  setHighlight(1);
}

BuildOptionsBox::~BuildOptionsBox()
{
  unsigned int i, nitems;

  nitems = numItems();
  for ( i = 0; i < nitems; i++ )
  {
    if (_items[i]->itemType() == "Label")
    {
      if ( (_items[i]->name()[0] != '+') &&
           (_items[i]->name()[0] != '-') )
        delete _items[i];
    }
  }
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void BuildOptionsBox::setBuild(const BuildListItem & build)
{
  std::vector<std::string> entries_txt;
  unsigned int nentries, i;

  _msg = build.name() + " build options";
  
  entries_txt = split(build.getProp("build_options"), ';');
  nentries = entries_txt.size();
  _entries.clear();
  _entries.resize(nentries);
  for ( i = 0; i < nentries; i++ )
  {
    if (_entries[i].width() == 0) { _entries[i].setWidth(30); }
    _entries[i].setText(entries_txt[i]);
  }
  setUp();
}

/*******************************************************************************

Get attributes

*******************************************************************************/
unsigned int BuildOptionsBox::numEntries() const { return _entries.size(); }
std::string BuildOptionsBox::entry(unsigned int idx) const
{
  return _entries[idx].text();
}

std::string BuildOptionsBox::entries() const
{
  int i, nentries;
  std::string entries_list;

  entries_list = "";
  nentries = numEntries();
  for ( i = 0; i < nentries-1; i++ )
  {
    if (_entries[i].text() != "") { entries_list += _entries[i].text() + ";"; }
  }
  if (nentries > 0)
  {
    if (_entries[nentries-1].text() != "")
      entries_list += _entries[nentries-1].text();
  }

  return entries_list;
}

/*******************************************************************************

Redraws box and entry

*******************************************************************************/
void BuildOptionsBox::draw(bool force)
{
  int rows, cols;
  unsigned int i, nentries;

  getmaxyx(_win, rows, cols);
  if (force) { _redraw_type = "all"; }

  // Set width of text boxes and horizontal lines

  if (_redraw_type == "all")
  {
    nentries = _entries.size();
    for ( i = 0; i < nentries; i++ )
    {
      _entries[i].setWidth(cols-2);
      _items[nentries+1+i]->setWidth(cols-2);
    }
    if (nentries > 0) { _items[numItems()-1]->setWidth(cols-2); }
  }

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

/*******************************************************************************

User interaction with input items in the box

*******************************************************************************/
std::string BuildOptionsBox::exec()
{
  bool getting_input;
  int y_offset, check_redraw;
  std::string selection, retval;

  getting_input = true;
  while (getting_input)
  {
    // Draw input box elements
  
    draw();
    _redraw_type = "changed";
    y_offset = _firstprint - _header_rows;

    // Get user input from highlighted item

    selection = _items[_highlight]->exec(y_offset);
    if (selection == signals::resize)
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if ( (selection == signals::quit) || (selection == signals::keyEnter) )
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if (selection == signals::keySpace)
    {
      if (_items[_highlight]->name() == "+ Add (press space)")
        addEntry();
      else if (_items[_highlight]->name() == "- Remove last (press space)")
        removeLast();
      retval = signals::resize;
      _redraw_type = "all";
      getting_input = false;
    }
    else if (selection == signals::highlightFirst)
    { 
      if (highlightFirst() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightLast) 
    { 
      if (highlightLast() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightPrevPage)
    {
      if (highlightPreviousPage() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightNextPage)
    {
      if (highlightNextPage() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightPrev)
    { 
      if (_highlight == _first_selectable)
        check_redraw = highlightFirst();
      else { check_redraw = highlightPrevious(); }
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightNext)
    {
      if (_highlight == _last_selectable)
        check_redraw = highlightLast();
      else { check_redraw = highlightNext(); }
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
  }

  return retval;
}
