#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "ScrollBox.h"

/*******************************************************************************

Displays build order

*******************************************************************************/
class BuildOrderBox: public ScrollBox {

  protected:

    std::string _info;

    /* Drawing */
    
    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    BuildOrderBox();
    BuildOrderBox(WINDOW *win, const std::string & name);

    /* Set attributes */

    void setInfo(const std::string & info);

    /* Get attributes */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;

    /* Creates list based on SlackBuild selected */

    virtual int create(BuildListItem & build,
                       std::vector<BuildListItem> & slackbuilds);

    /* Draws frame, items, etc. as needed */
 
    void draw(bool force=false);

    /* User interaction loop */

    std::string exec();
};
