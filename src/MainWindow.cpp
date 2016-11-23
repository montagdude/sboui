#include <iostream>

#include <vector>
#include <string>
#include <cmath>     // floor
#include "curses.h"
#include "Color.h"
#include "color_settings.h"
#include "backend.h"
#include "CategoryListBox.h"
#include "BuildListBox.h"
#include "BuildListItem.h"
#include "MainWindow.h"

using namespace color;

/*******************************************************************************

Prints to end of line, padding with spaces

*******************************************************************************/
void MainWindow::printToEol(const std::string & msg) const
{
  int i, y, x, rows, cols, nspaces, msglen;

  getmaxyx(stdscr, rows, cols);
  getyx(stdscr, y, x);

  msglen = msg.size();
  if (msglen > cols-x) { printw(msg.substr(0, cols-x).c_str()); }
  else
  {
    nspaces = std::max(cols-x-msglen, 0);
    printw(msg.c_str());
    for ( i = 0; i < nspaces; i++ ) { printw(" "); }
  }
}

/*******************************************************************************

Prints/clears status message

*******************************************************************************/
void MainWindow::printStatus(const std::string & msg) const
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  printToEol(msg);
  refresh();
}

void MainWindow::clearStatus() const
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  refresh();
}

/*******************************************************************************

Redraws header and footer

*******************************************************************************/
void MainWindow::redrawHeaderFooter() const
{
  int rows, cols;
  int pair_title, pair_info;

  getmaxyx(stdscr, rows, cols);

  // Draw title

  move(0, 0);
  clrtoeol();
  pair_title = colors.pair(fg_title, bg_title);
  if (pair_title != -1) { attron(COLOR_PAIR(pair_title)); }
  attron(A_BOLD);
  printToEol(_title);
  if (pair_title != -1) { attroff(COLOR_PAIR(pair_title)); }
  attroff(A_BOLD);

  // Print filter selection

  move(2, 0);
  clrtoeol();
  printToEol("Showing: " + _filter);
  refresh();

  // Draw footer

  move(rows-1, 0);
  clrtoeol();
  pair_info = colors.pair(fg_info, bg_info);
  if (pair_info != -1) { attron(COLOR_PAIR(pair_info)); }
  attron(A_BOLD);
  printToEol(_info);
  if (pair_info != -1) { attroff(COLOR_PAIR(pair_info)); }
  attroff(A_BOLD);
}

/*******************************************************************************

Redraws windows

*******************************************************************************/
void MainWindow::redrawWindows(bool force)
{
  int rows, cols;
  int listrows, leftlistcols, rightlistcols, xrightlist;
  const int minlistcols = 5;
  const int minlistrows = 3;

  getmaxyx(stdscr, rows, cols);

  // Set window dimensions and positions

  listrows = std::max(rows-5, minlistrows);
  leftlistcols = std::max(int(std::floor(double(cols)/2.)), minlistcols);
  rightlistcols = std::max(cols - leftlistcols, minlistcols);
  xrightlist = leftlistcols;

  // Position windows

  mvwin(_win1, 3, 0);
  wresize(_win1, listrows, leftlistcols);
  mvwin(_win2, 3, xrightlist);
  wresize(_win2, listrows, rightlistcols); 

  // Redraw windows

  _clistbox.draw(force);
  _blistboxes[_category_idx].draw(force);
}

/*******************************************************************************

Redraws window

*******************************************************************************/
void MainWindow::redrawAll(bool force)
{
  int rows, cols, i;

  getmaxyx(stdscr, rows, cols);

  // Clear everything except status line

  for ( i = 0; i < rows; i++ )
  {
    if ( i != rows-2 )
    {
      move(i, 0);
      clrtoeol();
    }
  }

  // Draw stuff

  redrawHeaderFooter(); 
  redrawWindows(force);
  refresh();
}

/*******************************************************************************

Constructor

*******************************************************************************/
MainWindow::MainWindow()
{
  _win1 = NULL;
  _win2 = NULL;
  _blistboxes.resize(0);
  _slackbuilds.resize(0);
  _categories.resize(0);
  _title = "sboui Development Version";
  _filter = "All";
  _info = "f: Filter | s: Search | o: Options | F1: Help";
  _category_idx = 0;
  _activated_listbox = 0;
}

/*******************************************************************************

First time window setup

*******************************************************************************/
int MainWindow::initialize()
{
  BuildListBox initlistbox;
  int retval;

  // Create windows (note: geometry gets set in redrawWindows);

  _win1 = newwin(4, 0, 10, 10);
  _win2 = newwin(4, 11,10, 10);

  _clistbox.setWindow(_win1);
  _clistbox.setActivated(true);
  _clistbox.setName("Categories");
  initlistbox.setWindow(_win2);
  initlistbox.setActivated(false);
  initlistbox.setName("SlackBuilds");
  _blistboxes.push_back(initlistbox);

  redrawAll();

  printStatus("Reading SlackBuilds repository ...");
  retval = readLists();
  if (retval != 0) { printStatus("Error reading SlackBuilds repository."); }
  else
  {
    rebuildListBoxes();
    clearStatus();
  }

  return retval;
}

/*******************************************************************************

Creates master list of SlackBuilds

*******************************************************************************/
int MainWindow::readLists()
{
  int check;
  unsigned int i, j, nbuilds, ncategories;
  bool new_category;

  // Get list of SlackBuilds

  check = read_repo(_slackbuilds); 
  if (check != 0) { return check; }

  // Create list of categories

  nbuilds = _slackbuilds.size();
  for ( i = 0; i < nbuilds; i++ )
  { 
    ncategories = _categories.size();
    new_category = true;
    for ( j = 0; j < ncategories; j++ )
    {
      if (_categories[j].name() == _slackbuilds[i].category())
      {
        new_category = false;
        break;
      }
    }
    if (new_category)
    {
      BuildListItem citem;
      citem.setName(_slackbuilds[i].category());
      citem.setCategory(_slackbuilds[i].category());
      _categories.push_back(citem);
    } 
  }

  return 0;
}

/*******************************************************************************

Populates list boxes

*******************************************************************************/
void MainWindow::rebuildListBoxes()
{
  unsigned int i,j, nbuilds, ncategories;

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  nbuilds = _slackbuilds.size();
  ncategories = _categories.size();
  _blistboxes.resize(0);
  for ( j = 0; j < ncategories; j++ )
  {
    _clistbox.addItem(&_categories[j]);
    BuildListBox blistbox;
    blistbox.setWindow(_win2);
    blistbox.setName(_categories[j].name());
    blistbox.setActivated(false);
    _blistboxes.push_back(blistbox);
  }
  for ( i = 0; i < nbuilds; i++ )
  {
    for ( j = 0; j < ncategories; j++ )
    {
      if (_slackbuilds[i].category() == _categories[j].name())
      {
        _blistboxes[j].addItem(&_slackbuilds[i]);
      }
    }
  }
}

/*******************************************************************************

Sets properties

*******************************************************************************/
void MainWindow::setTitle(const std::string & title) { _title = title; }
void MainWindow::setInfo(const std::string & info) { _info = info; }

/*******************************************************************************

Shows the main window

*******************************************************************************/
void MainWindow::show()
{
  std::string selection;
  bool getting_input, all_tagged;

  redrawAll();

  // Main event loop

  getting_input = true;
  while (getting_input)
  {
    // Get input from Categories list box

    if (_activated_listbox == 0)
    {
      selection = _clistbox.exec();
      if (selection == ListBox::highlightSignal)
      {
        _category_idx = _clistbox.highlight(); 
        _blistboxes[_category_idx].draw(true);
      }
      else if (selection == ListBox::keyTabSignal)
      {
        _clistbox.setActivated(false);
        _clistbox.draw();
        _blistboxes[_category_idx].setActivated(true);
        _activated_listbox = 1;
      }
      else if (selection == ListBox::tagSignal)
      {
        _blistboxes[_category_idx].tagAll();
        _blistboxes[_category_idx].draw();
      }
    }

    // Get input from SlackBuilds list box

    else if (_activated_listbox == 1)
    {
      selection = _blistboxes[_category_idx].exec();
      if (selection == ListBox::keyTabSignal)
      {
        _blistboxes[_category_idx].setActivated(false);
        _blistboxes[_category_idx].draw();
        _clistbox.setActivated(true);
        _activated_listbox = 0;
      }

      // Tag signal: see if we need to change tag for category

      else if (selection == ListBox::tagSignal)
      {
        all_tagged = _blistboxes[_category_idx].allTagged();
        if (_categories[_category_idx].tagged())
        {
          if (! all_tagged) 
          { 
            _categories[_category_idx].setTagged(false); 
            _clistbox.draw();
          }
        }
        else 
        {
          if (all_tagged) 
          { 
            _categories[_category_idx].setTagged(true); 
            _clistbox.draw();
          }
        }
      }
    }

    // Key signals with the same action w/ either type of list box

    if (selection == ListBox::quitSignal) { getting_input = false; }
    else if (selection == ListBox::resizeSignal) 
    { 
      redrawAll(true); 
      clearStatus();
    }
  }
}
