#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "AbstractListBox.h"

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
                   
    /* Determines first item to print (needed for scrolling) */
    
    int determineFirstPrint();

    /* Prints to end of line, padding with spaces and avoiding borders */

    void printToEol(const std::string & msg) const;
    void printSpaces(unsigned int nspaces) const;

    /* Drawing */
    
    virtual void redrawFrame() const;
    virtual void redrawSingleItem(unsigned int idx);
    void redrawChangedItems();
    void redrawAllItems();

  public:

    /* Constructors */

    ListBox();
    ListBox(WINDOW *win, const std::string & name);

    /* Edit list */

    void removeItem(unsigned int idx);
    void clearList();

    /* Set attributes */

    void setActivated(bool activated);

    /* Get attributes */

    unsigned int highlight() const;
    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;

    /* Returns pointer to item */

    ListItem * highlightedItem();

    /* Draws frame, items, etc. as needed */
 
    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec();
};
