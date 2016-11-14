#pragma once

#include <curses.h>
#include <string>
#include "CategoryListBox.h"
#include "ListBox.h"
#include "SlackBuildListItem.h"

/*******************************************************************************

Main window

*******************************************************************************/
class MainWindow {

  private:

    WINDOW *_win1, *_win2;
    CategoryListBox _clist;
    std::vector<ListBox> _blists;
    std::vector<SlackBuildListItem> _slackbuilds;
    std::vector<CategoryListItem> _categories;
    std::string _title, _filter, _info;

    void printToEol(const std::string & msg) const;

    void redrawHeaderFooter() const;
    void redrawWindows() const;
    void redrawAll() const;

  public:

    /* Constructor */

    MainWindow();

    /* First time window setup */

    void initialize();

    /* Set properties */

    void setTitle(const std::string & title);
    void setFilter(const std::string & filter);
    void setInfo(const std::string & info);

    /* Shows the main window */

    void show();
};
