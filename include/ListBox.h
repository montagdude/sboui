#pragma once

#include <string>
#include <vector>
#include <curses.h>

class ListItem;

/*******************************************************************************

List box with selectable items

*******************************************************************************/
class ListBox {

  protected:

    WINDOW *_win;
    std::string _name, _redraw_type;
    std::vector<ListItem *> _items;
    unsigned int _highlight, _firstprint, _prevhighlight;
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

    /* Drawing */
    
    virtual void redrawFrame() const;
    virtual void redrawSingleItem(unsigned int idx);
    void redrawChangedItems();
    void redrawAllItems();

  public:

    /* Constructors */

    ListBox();
    ListBox(WINDOW *win, const std::string & name);

    /* Signals returned by different keystrokes */

    static std::string resizeSignal;
    static std::string quitSignal;
    static std::string keyRightSignal;
    static std::string keyLeftSignal;
    static std::string tagSignal;
    static std::string highlightSignal;

    /* Edit list */

    void addItem(ListItem *item);
    void removeItem(unsigned int idx);
    void clearList();

    /* Set attributes */

    void setWindow(WINDOW *win);
    void setName(const std::string & name);
    void setActivated(bool activated);

    /* Get attributes */

    const std::string & name() const;
    unsigned int highlight() const;

    /* Draws list box and all items */
 
    void draw();

    /* User interaction loop */

    virtual std::string exec();
};
