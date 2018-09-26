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

//FIXME: move as much of this stuff as possible up to AbstractListBox so that
// it can be reused by ScrollBox.

  protected:

    int _highlight, _prevhighlight;
    bool _activated;
    std::vector<std::string> _buttons;  // At the bottom, e.g. OK/Cancel
    std::vector<std::string> _button_signals;
    std::vector<int> _button_left;      // Left edge of each button
    std::vector<int> _button_right;     // Right edge of each button
    int _highlighted_button;
    std::string _bg_color, _fg_color;   // Default colors

    /* Setting item to be highlighted */

    int highlightFirst();
    int highlightLast();
    int highlightPrevious();
    int highlightNext();
    int highlightNextPage();
    int highlightPreviousPage();
    int highlightFractional(const double & frac);
    int highlightNextButton();
    int highlightPreviousButton();

    /* Determines first item to print (needed for scrolling) */

    int determineFirstPrint();

    /* Drawing */

    virtual void redrawFrame();
    virtual void redrawButtons();
    virtual void redrawSingleItem(unsigned int idx);
    virtual void redrawScrollIndicator() const;
    void redrawChangedItems();
    void redrawAllItems();

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
    void addButton(const std::string & button, const std::string & signal);
    void setColor(const std::string & fg_color, const std::string & bg_color);

    /* Get attributes */

    unsigned int highlight() const;
    const std::string & highlightedName() const;
    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;
    int highlightedButton() const;
    const std::string & fgColor() const;
    const std::string & bgColor() const;

    /* Sets highlight by search for string */

    int highlightSearch(const std::string & pattern); 

    /* Returns pointer to item */

    ListItem * highlightedItem();

    /* Handles mouse event */

    std::string handleMouseEvent(MouseEvent * mevent);

    /* Draws frame, items, etc. as needed */
 
    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL);
};
