#include <string>
#include <sstream>
#include <cmath>     // floor
#include "curses.h"
#include "colors.h"
#include "ListBox.h"
#include "MainWindow.h"

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

  getmaxyx(stdscr, rows, cols);

  // Draw title

  move(0, 0);
  clrtoeol();
  if (colors::title != -1) { attron(COLOR_PAIR(colors::title)); }
  printToEol(_title);
  if (colors::title != -1) { attroff(COLOR_PAIR(colors::title)); }

  // Print filter selection

  move(2, 0);
  clrtoeol();
  printToEol("Filter: " + _filter);
  refresh();

  // Draw footer

  move(rows-1, 0);
  clrtoeol();
  if (colors::info != -1) { attron(COLOR_PAIR(colors::info)); }
  printToEol(_info);
  if (colors::info != -1) { attroff(COLOR_PAIR(colors::info)); }
}

/*******************************************************************************

Redraws windows

*******************************************************************************/
void MainWindow::redrawWindows() const
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

  _leftlist->draw();
  _rightlist->draw();
}

/*******************************************************************************

Redraws window

*******************************************************************************/
void MainWindow::redrawAll() const
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
  _title = "sboui";
  _filter = "All";
  _info = "F1: Help";
}

/*******************************************************************************

First time window setup

*******************************************************************************/
void MainWindow::initialize()
{
  unsigned int i;

  // Create windows (note: geometry gets set in redrawWindows);

  _win1 = newwin(4, 0, 10, 10);
  _win2 = newwin(4, 11,10, 10);

  // Create lists and set pointers

  for ( i = 0; i < 10; i++ ) 
                           { _allcategories.addItem("Category " + int2str(i)); }
  _allcategories.setWindow(_win1);
  _allcategories.setName("Categories");
  _allcategories.setActivated(true);

  for ( i = 0; i < 30; i++ ) { _allbuilds.addItem("SlackBuild " + int2str(i)); }
  _allbuilds.setWindow(_win2);
  _allbuilds.setName("SlackBuilds");
  _allbuilds.setActivated(false);

  _leftlist = &_allcategories;
  _rightlist = &_allbuilds;
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
  bool display;
  unsigned int curlist;
  std::string selection;

  redrawAll();

  // Main event loop

  curlist = 0;
  display = true;
  while (display)
  {
    if (curlist == 0)
    {
      selection = _leftlist->exec();
      if (selection == ListBox::keyRightSignal)
      {
        _leftlist->setActivated(false);
        _leftlist->draw();
        _rightlist->setActivated(true);
        curlist = 1; 
      }
    }
    else if (curlist == 1)
    {
      selection = _rightlist->exec();
      if (selection == ListBox::keyLeftSignal)
      {
        _leftlist->setActivated(true);
        _rightlist->setActivated(false);
        _rightlist->draw();
        curlist = 0; 
      }
    }
    if (selection == ListBox::quitSignal) { display = false; }
    else if (selection == ListBox::resizeSignal) { redrawAll(); }
  }
}
