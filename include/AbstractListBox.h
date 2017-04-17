#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "CursesWidget.h"

class ListItem;

/*******************************************************************************

Template list box class

*******************************************************************************/
class AbstractListBox: public CursesWidget {

  protected:

    std::string _name, _redraw_type;
    std::vector<ListItem *> _items;
    unsigned int _reserved_rows, _header_rows;
    int _firstprint;

    virtual void redrawFrame() const = 0;
    virtual void redrawSingleItem(unsigned int idx) = 0;

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

    /* Get attributes */

    const std::string & name() const;
    unsigned int numItems() const;

    /* Returns pointer to item */

    ListItem * itemByIdx(unsigned int idx);
};
