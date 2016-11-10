#pragma once

#include <string>
#include <vector>
#include <curses.h>

struct listitem {
  std::string name;
  bool tagged;
};

/*******************************************************************************

List box with selectable items

*******************************************************************************/
class ListBox {

  protected:

    WINDOW *_win;
    std::string _name;
    std::vector<listitem> _items;
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
    
    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);
    void redrawChangedItems();
    void redrawAllItems();

  public:

    /* Constructor */

    ListBox();
    ListBox(WINDOW *win, const std::string & name);

    /* Signal returned by show() when window is resized */

    static std::string resizeSignal;

    /* Edit list */

    void addItem(const std::string & name, bool tagged=false);
    void removeItem(unsigned int idx);
    void toggleItemTag(unsigned int idx);
    void clearList();

    /* Set attributes */

    void setWindow(WINDOW *win);
    void setName(const std::string & name);
    void setActivated(bool activated);

    /* Get attributes */

    const std::string & name() const;

    /* Draws list box and all items */
 
    void draw();

    /* User interaction loop */

    std::string exec();
};
