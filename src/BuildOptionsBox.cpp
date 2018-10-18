#include <string>
#include <curses.h>
#include <fstream>
#include <cstdio>	// remove
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "string_util.h"
#include "BuildListItem.h"
#include "TextInput.h"
#include "Label.h"
#include "DirListing.h"
#include "BuildOptionsBox.h"
#include "MouseEvent.h"

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

Constructor

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

Writes build options to file. Deletes file if it exists and no build options
are set.

*******************************************************************************/
int BuildOptionsBox::write(const BuildListItem & build) const
{
  DirListing listing;
  std::ofstream file;
  std::string fname;
  unsigned int check, nentries, i;

  check = listing.createFromPath("/var/lib/sboui/buildopts");
  if (check != 0) { return 1; }
  fname = listing.path() + build.name() + ".buildopts";
  nentries = numEntries();
  file.open(fname.c_str());
  if (not file.is_open()) { return 2; }

  // Delete existing file if there are no entries or write to file

  if (nentries == 0) { remove(fname.c_str()); }
  else
  {
    for ( i = 0; i < nentries; i++ )
    {
      file << _entries[i].text() << std::endl;
    }
  }
  file.close();

  return 0;
}

/*******************************************************************************

Redraws box and entry

*******************************************************************************/
void BuildOptionsBox::draw(bool force)
{
  int rows, cols;
  unsigned int i, nitems;

  getmaxyx(_win, rows, cols);
  if (force) { _redraw_type = "all"; }

  // Set width of everything

  if (_redraw_type == "all")
  {
    nitems = numItems();
    for ( i = 0; i < nitems; i++ )
    {
      _items[i]->setWidth(cols-2);
    }
  }
  InputBox::draw(force);
}

/*******************************************************************************

User interaction with input items in the box

*******************************************************************************/
std::string BuildOptionsBox::exec(MouseEvent * mevent)
{
  bool getting_input, needs_selection;
  int y_offset;
  std::string selection, retval;

  getting_input = true;
  needs_selection = true;
  while (getting_input)
  {
    // Draw input box elements
  
    draw();
    _redraw_type = "changed";
    y_offset = _firstprint - _header_rows;

    // Get user input from highlighted item

    if (needs_selection)
      selection = _items[_highlight]->exec(y_offset, mevent);
    else
      needs_selection = true;

    retval = handleInput(selection, getting_input, needs_selection, mevent);

    if (retval == signals::keySpace)  // FIXME: use buttons for this
    {
      if (_items[_highlight]->name() == "+ Add (press space)")
        addEntry();
      else if (_items[_highlight]->name() == "- Remove last (press space)")
        removeLast();
      retval = signals::resize;
      _redraw_type = "all";
      getting_input = false;
    }
  }

  return retval;
}
