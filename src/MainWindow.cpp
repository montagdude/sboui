#include <vector>
#include <string>
#include <sstream>
#include <cmath>     // floor
#include "curses.h"
#include "Color.h"
#include "color_settings.h"
#include "ListBox.h"
#include "SlackBuildListItem.h"
#include "CategoryListItem.h"
#include "MainWindow.h"

using namespace color;

std::string int2str(int inval)
{
  std::stringstream ss;
  ss << inval;
  return ss.str();
}

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
  attron(A_BOLD);
}

/*******************************************************************************

Redraws windows

*******************************************************************************/
void MainWindow::redrawWindows()
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

  _clistbox.draw();
  _blistboxes[_category_idx].draw();
}

/*******************************************************************************

Redraws window

*******************************************************************************/
void MainWindow::redrawAll()
{
  clear();
  redrawHeaderFooter(); 
  redrawWindows();
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
  _title = "SlackBuilds Browser";
  _filter = "All";
  _info = "F1: Help";
  _category_idx = 0;
  _activated_listbox = 0;
}

/*******************************************************************************

First time window setup

*******************************************************************************/
void MainWindow::initialize()
{
  unsigned int i, j, nbuilds;
  std::vector<std::string> builds;

  // Create windows (note: geometry gets set in redrawWindows);

  _win1 = newwin(4, 0, 10, 10);
  _win2 = newwin(4, 11,10, 10);

  // Create master lists

  for ( i = 0; i < 10; i++ )
  {
    CategoryListItem citem;
    citem.setName("Category " + int2str(i));
    _categories.push_back(citem);
    for ( j = 0; j < 30; j++ )
    {
      SlackBuildListItem bitem;
      bitem.setCategory(citem.name());
      bitem.setName(bitem.category() + ", SlackBuild " + int2str(j));
      _slackbuilds.push_back(bitem);
    }
  }

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  nbuilds = 0;
  _clistbox.setWindow(_win1);
  _clistbox.setActivated(true);
  _clistbox.setName("Categories");
  for ( i = 0; i < 10; i++ )
  {
    _clistbox.addCategory(&_categories[i]);
    ListBox blistbox;
    blistbox.setWindow(_win2);
    blistbox.setName(_categories[i].name());
    blistbox.setActivated(false);
    for ( j = 0; j < 30; j++ )
    {
      blistbox.addItem(&_slackbuilds[nbuilds]);
      nbuilds++;
    }
    _blistboxes.push_back(blistbox);
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
  bool getting_input;

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
        _blistboxes[_category_idx].draw();
      }
      else if (selection == ListBox::keyRightSignal)
      {
        _clistbox.setActivated(false);
        _clistbox.draw();
        _blistboxes[_category_idx].setActivated(true);
        _activated_listbox = 1;
      }
    }

    // Get input from SlackBuilds list box

    else if (_activated_listbox == 1)
    {
      selection = _blistboxes[_category_idx].exec();
      if (selection == ListBox::keyLeftSignal)
      {
        _blistboxes[_category_idx].setActivated(false);
        _blistboxes[_category_idx].draw();
        _clistbox.setActivated(true);
        _activated_listbox = 0;
      }
    }

    // Key signals with the same action w/ either type of list box

    if (selection == ListBox::quitSignal) { getting_input = false; }
    else if (selection == ListBox::resizeSignal) { redrawAll(); }
  }
}
