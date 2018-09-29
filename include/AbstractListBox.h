#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "MouseEvent.h"
#include "CursesWidget.h"

class ListItem;

/*******************************************************************************

Template list box class. Used by both ListBox class (selectable items) and
ScrollBox class (non-selectable items).

*******************************************************************************/
class AbstractListBox: public CursesWidget {

  protected:

    std::string _name, _redraw_type;
    std::vector<ListItem *> _items;
    int _firstprint;
    std::string _bg_color, _fg_color;   // Default colors

    virtual void redrawFrame();
    virtual void redrawButtons();
    virtual void redrawSingleItem(unsigned int idx) = 0;
    virtual void redrawScrollIndicator() const = 0;
    virtual void redrawAllItems();

  public:

    /* Constructors */

    AbstractListBox();
    AbstractListBox(WINDOW *win, const std::string & name);

    /* Edit list */

    virtual void addItem(ListItem *item);
    virtual void removeItem(unsigned int idx);
    virtual void clearList();

    /* Set attributes */

    void setName(const std::string & name);
    void setColor(const std::string & fg_color, const std::string & bg_color);

    /* Get attributes */

    const std::string & name() const;
    unsigned int numItems() const;
    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;
    const std::string & fgColor() const;
    const std::string & bgColor() const;

    /* Returns pointer to item */

    ListItem * itemByIdx(unsigned int idx);

    /* Handles mouse event */

    std::string handleMouseEvent(MouseEvent *mevent) = 0;

    /* Draws frame, items, etc. as needed */

    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL) = 0;
};
