#pragma once

#include <curses.h>
#include <vector>
#include <string>
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "FilterBox.h"
#include "SearchBox.h"
#include "InputBox.h"

/*******************************************************************************

Main window

*******************************************************************************/
class MainWindow {

  private:

    WINDOW *_win1, *_win2;
    CategoryListBox _clistbox;
    std::vector<BuildListBox> _blistboxes;
    std::vector<BuildListItem> _slackbuilds;
    std::vector<BuildListItem *> _installedlist, _nondeplist;
    std::vector<CategoryListItem> _categories;
    FilterBox _fbox;
    SearchBox _searchbox;
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
    void filterNonDeps();
    void filterSearch(const std::string & searchterm, bool case_sensitive=false,
                      bool whole_whord=false);

    /* Displays build order for a given SlackBuild */

    void showBuildOrder(const BuildListItem & build) const;

    /* Sets size of popup boxes */

    void popupSize(int & height, int & width, ListBox *popup) const;
    void popupSize(int & height, int & width, InputBox *popup) const;
    void placePopup(ListBox *popup, WINDOW *win) const;
    void placePopup(InputBox *popup, WINDOW *win) const;

  public:

    /* Constructor and destructor */

    MainWindow();
    ~MainWindow();

    /* Window setup */

    int initialize();
    int readLists();

    /* Set properties */

    void setTitle(const std::string & title);
    void setInfo(const std::string & info);

    /* Dialogs */

    void selectFilter();
    void search();
    void showBuildActions(const BuildListItem & build);

    /* Shows the main window */

    void show();
};
