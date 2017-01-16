#pragma once

#include <string>
#include <vector>
#include <curses.h>

class ListItem;

/*******************************************************************************

Template list box class

*******************************************************************************/
class AbstractListBox {

  protected:

    WINDOW *_win;
    std::string _name, _redraw_type;
    std::vector<ListItem *> _items;
    unsigned int _reserved_rows;
    int _firstprint;

    /* Prints to end of line, padding with spaces and avoiding borders */

    virtual void printToEol(const std::string & msg) const;
    void printSpaces(unsigned int nspaces) const;

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

    void setWindow(WINDOW *win);
    void setName(const std::string & name);

    /* Get attributes */

    const std::string & name() const;
    unsigned int numItems() const;
    virtual void minimumSize(int & height, int & width) const = 0;
    virtual void preferredSize(int & height, int & width) const = 0;

    /* Returns pointer to item */

    ListItem * itemByIdx(unsigned int idx);

    /* Draws frame, items, etc. as needed */
 
    virtual void draw(bool force=false) = 0;

    /* User interaction loop */

    virtual std::string exec() = 0;
};
