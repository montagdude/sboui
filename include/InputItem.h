#pragma once

#include <string>
#include <curses.h>

/*******************************************************************************

Basic class for an item that goes in an input box, such as text input or
switch. This class serves as a template.

*******************************************************************************/
class InputItem {

  protected:

    WINDOW *_win;
    int _posx, _posy, _width; 
    std::string _name, _redraw_type;

    /* Prints to end of line */

    void printToEol(const std::string & msg) const;

  public:

    InputItem();

    // Set attributes

    void setName(const std::string & name);
    void setPosition(int x, int y);
    void setWidth(int width);
    void setWindow(WINDOW *win);

    // Get attributes

    const std::string & name() const;

    // User interaction

    virtual void draw(bool force=false, bool highlight=false) = 0;
    virtual std::string exec() = 0;

    // Accessing properties of different types. Derived classes should
    // reimplement these as needed.

    virtual std::string getStringProp() const;
    virtual bool getBoolProp() const;
    virtual int getIntProp() const;
    virtual double getDoubleProp() const;
};
