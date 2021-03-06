#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "AbstractListBox.h"
#include "MouseEvent.h"

class ListItem;

/*******************************************************************************

List box with selectable items

*******************************************************************************/
class ListBox: public AbstractListBox {

  protected:

    int _highlight, _prevhighlight;
    bool _activated;

    /* Setting item to be highlighted */

    int highlightFirst();
    int highlightLast();
    int highlightPrevious();
    int highlightNext();
    int highlightNextPage();
    int highlightPreviousPage();
    int highlightFractional(const double & frac);

    /* Determines first item to print (needed for scrolling) */

    int determineFirstPrint();

    /* Drawing */

    virtual void redrawSingleItem(unsigned int idx);
    virtual void redrawScrollIndicator() const;
    void redrawChangedItems();

  public:

    /* Constructors */

    ListBox();
    ListBox(WINDOW *win, const std::string & name);

    /* Edit list */

    void removeItem(unsigned int idx);
    virtual void clearList();

    /* Set attributes */

    void setActivated(bool activated);
    int setHighlight(int highlight);
    int setHighlight(const std::string & name);

    /* Get attributes */

    unsigned int highlight() const;
    const std::string & highlightedName() const;

    /* Sets highlight by search for string */

    int highlightSearch(const std::string & pattern); 

    /* Returns pointer to item */

    ListItem * highlightedItem();

    /* Handles mouse event */

    virtual std::string handleMouseEvent(MouseEvent * mevent);

    /* Draws frame, items, etc. as needed */
 
    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL);
};
