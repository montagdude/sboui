#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "MouseEvent.h"
#include "MenubarList.h"
#include "CursesWidget.h"

class MenubarList;

/*******************************************************************************

Menubar class

*******************************************************************************/
class Menubar: public CursesWidget {

  protected:

    int _highlight, _prevhighlight;
    unsigned int _pad, _outerpad, _innerpad;
    bool _activated;
    std::vector<MenubarList> _lists;
    std::vector<WINDOW *> _listwins;
    CursesWidget *_parent;

    unsigned int menuColumn(unsigned int lidx) const;
    void placeListBox();

    void highlightFirst();
    void highlightLast();
    void highlightPrevious();
    void highlightNext();
    void setHighlight(int highlight);

    void redrawSingleItem(unsigned int idx);
    void redrawChangedItems();
    void redrawAllItems();

    std::string execList(MouseEvent * mevent=NULL);

  public:

    /* Constructors and destructor */

    Menubar();
    Menubar(WINDOW *win);
    ~Menubar();

    /* Set attributes */

    void setActivated(bool activated);
    void setParent(CursesWidget * parent);
    void setPad(unsigned int pad);
    void setListPad(unsigned int outerpad, unsigned int innerpad);
    int addList(const std::string & lname, int hotkey=-1);
    int addListItem(unsigned int lidx, const std::string & text,
                    const std::string & shortcut="", int hotkey=-1);
    int addListItem(const std::string & lname, const std::string & text,
                    const std::string & shortcut="", int hotkey=-1);
    void clearLists();

    /* Get attributes */

    unsigned int numLists() const;
    unsigned int highlightedList() const;
    const std::string & highlightListName() const;
    const std::string & highlightedListItem() const;
    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;

    /* Handles mouse event */

    std::string handleMouseEvent(MouseEvent * mevent);

    /* Draws frame, items, etc. as needed */

    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL);
};
