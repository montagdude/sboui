#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "CursesWidget.h"
#include "MouseEvent.h"

class InputItem;

/*******************************************************************************

Box containing a number of InputItems

*******************************************************************************/
class InputBox: public CursesWidget {

  protected:

    std::string _msg;
    std::vector<InputItem *> _items;    
    int _highlight, _prevhighlight, _firstprint;
    int _header_rows, _first_selectable, _last_selectable;
    int _color_idx;   // TODO: use method from CursesWidget
    bool _has_scroll_indicator;

    /* Setting item to be highlighted */

    int highlightFirst();
    int highlightLast();
    int highlightPrevious();
    int highlightNext();
    int highlightPreviousPage();
    int highlightNextPage();
    int highlightFractional(const double & frac);

    /* Determines first line to print (needed for scrolling) */
    
    int determineFirstPrint();

    /* Drawing */
    
    virtual void redrawFrame();
    virtual void redrawScrollIndicator() const;
    void redrawChangedItems(bool force);
    void redrawAllItems(bool force);

  public:

    /* Constructors */

    InputBox();
    InputBox(WINDOW *win, const std::string & msg);

    /* Add/remove items */

    void addItem(InputItem *item);
    void clear();

    /* Set attributes */

    void setWindow(WINDOW *win);
    void setMessage(const std::string & msg);
    void setHighlight(unsigned int highlight);
    void setColor(int color_idx); // TODO: use method from CursesWidget

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;
    unsigned int numItems() const;

    /* Mouse interaction */

    std::string handleMouseEvent(MouseEvent * mevent);

    /* Draws frame, entry, etc. as needed */

    virtual void draw(bool force=false);

    /* User interaction loop */

    std::string handleInput(std::string & selection, bool & getting_input,
                            bool & needs_selection, MouseEvent * mevent=NULL);
    virtual std::string exec(MouseEvent * mevent=NULL);
};
