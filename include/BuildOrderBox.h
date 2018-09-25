#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "BuildListBox.h"

/*******************************************************************************

Displays build order

*******************************************************************************/
class BuildOrderBox: public BuildListBox {

  protected:

    std::string _info;

    /* Drawing */
    
    virtual void redrawFrame();
    virtual void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    BuildOrderBox();
    BuildOrderBox(WINDOW *win, const std::string & name);

    /* Set attributes */

    void setInfo(const std::string & info);

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;

    /* Creates list based on SlackBuild selected */

    int create(BuildListItem & build,
               std::vector<std::vector<BuildListItem> > & slackbuilds);

    /* Draws frame, items, etc. as needed */
 
    void draw(bool force=false);
};
