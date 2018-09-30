#pragma once

#include <curses.h>
#include <vector>
#include <string>
#include "CursesWidget.h"
#include "AbstractListBox.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "FilterBox.h"
#include "SearchBox.h"
#include "InputBox.h"
#include "TagList.h"
#include "OptionsWindow.h"
#include "HelpWindow.h"
#include "MouseEvent.h"

/*******************************************************************************

Main window

*******************************************************************************/
class MainWindow: public CursesWidget {

  private:

    WINDOW *_win1, *_win2;
    CategoryListBox _clistbox;
    std::vector<BuildListBox> _blistboxes;
    std::vector<std::vector<BuildListItem> > _slackbuilds;
    std::vector<CategoryListItem> _categories;
    FilterBox _fbox;
    SearchBox _searchbox;
    TagList _taglist;
    OptionsWindow _options;
    HelpWindow _help;
    std::string _title, _filter, _info, _status;
    unsigned int _category_idx, _activated_listbox;

    void printStatus(const std::string & msg, bool bold=false);
    void clearStatus();
    void refreshStatus();

    void redrawHeaderFooter() const;
    void redrawWindowsHorz();
    void redrawWindowsVert();
    void redrawWindows(bool force=false);
    void toggleLayout();

    /* Clearing and setting up lists, etc. */

    void clearData();
    int readLists();
    void rebuild();

    /* Filters lists */

    void filterAll();
    void filterInstalled();
    void filterUpgradable();
    void filterTagged();
    void filterBlacklisted();
    void filterNonDeps();
    void filterBuildOptions();
    void filterSearch(const std::string & searchterm, bool case_sensitive=false,
                      bool whole_whord=false, bool search_readmes=false);

    /* Displays options window */

    int showOptions();

    /* Displays help window */

    int showHelp(MouseEvent * mevent=NULL);

    /* Sets taglist reference in BuildListBoxes */

    void setTagList();

    /* Actions for a selected SlackBuild */

    void browseFiles(const BuildListItem & build, MouseEvent * mevent=NULL);
    bool modifyPackage(BuildListItem & build, const std::string & action,
                       int & ninstalled, int & nupgraded, int & nreinstalled,
                       int & nremoved, bool & cancel_all, bool batch=false,
                       MouseEvent * mevent=NULL);
    void setBuildOptions(BuildListItem & build);
    void showBuildOrder(BuildListItem & build, MouseEvent * mevent=NULL);
    void showInverseReqs(const BuildListItem & build);
    void showPackageInfo(BuildListItem & build, MouseEvent * mevent=NULL);

    /* Sync/update */

    int syncRepo();

    /* Apply action to tagged SlackBuilds */

    void applyTags(const std::string & action);

    /* View command line output */
 
    void viewCommandLine() const;

    /* Displays an error message */

    std::string displayError(const std::string & msg, bool centered=true,
                             const std::string & name="Error",
                             const std::string & info="Enter: Dismiss");
    std::string displayMessage(const std::string & msg, bool centered=true,
                               const std::string & name="Information",
                               const std::string & info="Enter: Dismiss");

    /* Prints package version information as status */

    void printPackageVersion(const BuildListItem & build);

  public:

    /* Constructor and destructor */

    MainWindow(const std::string & version);
    ~MainWindow();

    /* Window setup */

    int initialize();

    /* Set properties */

    void setTitle(const std::string & title);
    void setInfo(const std::string & info);

    /* Dialogs */

    void selectFilter(MouseEvent * mevent=NULL);
    void search();
    void showBuildActions(BuildListItem & build, bool limited_actions=false,
                          MouseEvent * mevent=NULL);

    /* Quick search in active list */

    void quickSearch();

    /* Not used, but needed for MainWindow to be derived from CursesWidget */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;

    /* Mouse interaction */

    std::string handleMouseEvent(MouseEvent * mevent);

    /* Redraws the main window */

    void draw(bool force=false);

    /* Shows the main window */

    std::string exec(MouseEvent * mevent=NULL);
};
