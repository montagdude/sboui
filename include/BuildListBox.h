#pragma once

#include <string>
#include <curses.h>
#include "ListBox.h"
#include "MouseEvent.h"

class TagList;

/*******************************************************************************

ListBox for SlackBuilds

*******************************************************************************/
class BuildListBox: public ListBox {
  
  protected:

    TagList *_taglist;

    virtual void redrawFrame();
    virtual void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    BuildListBox();
    BuildListBox(WINDOW *win, const std::string & name);

    /* Tagging */

    void setTagList(TagList *taglist);
    void tagSlackBuild(unsigned int idx);
    unsigned int tagAll();

    /* Accessing properties */

    bool allTagged() const;

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL);
};
