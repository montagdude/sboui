#pragma once

#include <curses.h>
#include <string>
#include "CategoryListBox.h"
#include "ListBox.h"
#include "BuildListItem.h"
#include "CategoryListItem.h"

/*******************************************************************************

Main window

*******************************************************************************/
class MainWindow {

  private:

    WINDOW *_win1, *_win2;
    CategoryListBox _clistbox;
    std::vector<ListBox> _blistboxes;
    std::vector<BuildListItem> _slackbuilds;
    std::vector<CategoryListItem> _categories;
    std::string _title, _filter, _info;
    unsigned int _category_idx, _activated_listbox;

    void printToEol(const std::string & msg) const;

    void redrawHeaderFooter() const;
    void redrawWindows();
    void redrawAll();

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
