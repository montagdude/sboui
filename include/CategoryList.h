#pragma once

#include <vector>
#include <string>
#include "ListBox.h"

/*******************************************************************************

ListBox for categories, with each category containing a list of SlackBuilds.

*******************************************************************************/
class CategoryList: public ListBox {

  private:

    WINDOW *_buildlistwin;
    std::vector<ListBox> _buildlists;

  public:

    /* Constructor */

    CategoryList();
    CategoryList(WINDOW *win, WINDOW *buildlistwin, const std::string & name);

    /* Set attributes */

    void setBuildListWindow(WINDOW *buildlistwin);
    void addCategory(const std::string & name, 
                     const std::vector<std::string> & builds);

    /* User interaction loop */

    std::string exec();
};
