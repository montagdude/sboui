#pragma once

#include <curses.h>
#include <vector>
#include <string>
#include "CategoryListBox.h"
#include "BuildListBox.h"
#include "BuildListItem.h"

/*******************************************************************************

Main window

*******************************************************************************/
class MainWindow {

  private:

    WINDOW *_win1, *_win2;
    CategoryListBox _clistbox;
    std::vector<BuildListBox> _blistboxes;
    std::vector<BuildListItem> _slackbuilds;
    std::vector<BuildListItem> _categories;
    std::string _title, _filter, _info;
    unsigned int _category_idx, _activated_listbox;

    void printToEol(const std::string & msg) const;
    void printStatus(const std::string & msg) const;
    void clearStatus() const;

    void redrawHeaderFooter() const;
    void redrawWindows(bool force=false);
    void redrawAll(bool force=false);

    void filterAll();
    void filterInstalled();
    void filterUpgradable();

  public:

    /* Constructor and destructor */

    MainWindow();
    ~MainWindow();

    /* Window setup */

    int initialize();
    int readLists();

    /* Set properties */

    void setTitle(const std::string & title);
    void setFilter(const std::string & filter);
    void setInfo(const std::string & info);

    /* Shows the main window */

    void show();
};
