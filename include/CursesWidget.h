#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "MouseEvent.h"

/*******************************************************************************

Abstract class for CursesWidgets. Derived classes must implement sizing rules,
draw, and exec methods.

*******************************************************************************/
class CursesWidget {

  protected:

    void printToEol(const std::string & msg, int printable_cols=-1) const;
    void printSpaces(int nspaces) const;
    void clearWindow () const;
    virtual int highlightNextButton();
    virtual int highlightPreviousButton();
    virtual void redrawButtons();

    std::vector<std::string> _buttons;  // At the bottom, e.g. OK/Cancel
    std::vector<std::string> _button_signals;
    std::vector<int> _button_left;      // Left edge of each button
    std::vector<int> _button_right;     // Right edge of each button

    int _highlighted_button;
    unsigned int _reserved_rows, _header_rows;
    std::string _redraw_type;
    std::string _button_fg, _button_bg;
    std::string _bg_color, _fg_color;   // Default colors

    WINDOW *_win;

  public:

    /* Constructor */

    CursesWidget();

    /* Set attributes */

    virtual void setWindow(WINDOW *win);
    void addButton(const std::string & button, const std::string & signal);
    void clearButtons();
    void setButtons(const std::vector<std::string> & buttons,
                    const std::vector<std::string> & button_signals);
    void setButtonColor(const std::string & button_fg,
                        const std::string & button_bg);
    void setColor(const std::string & fg_color, const std::string & bg_color);

    /* Sets size and position of popup boxes */

    virtual void popupSize(int & height, int & width, CursesWidget *popup) const;
    virtual void placePopup(CursesWidget *popup, WINDOW *win) const;

    /* Hides a popup window by putting it at the center with 0 size */

    virtual void hideWindow(WINDOW *win) const;

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const = 0;
    virtual void preferredSize(int & height, int & width) const = 0;
    int highlightedButton() const;
    const std::string & fgColor() const;
    const std::string & bgColor() const;

    /* Mouse interaction */

    virtual std::string handleMouseEvent(MouseEvent * mevent) = 0;

    /* Draws frame and message */

    virtual void draw(bool force=false) = 0;

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL) = 0;
};
