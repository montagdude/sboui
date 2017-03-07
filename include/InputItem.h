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
    std::string _name, _redraw_type, _item_type;
    bool _selectable, _auto_position;

    /* Prints to end of line or specified number of spaces */

    void printToEol(const std::string & msg) const;
    void printSpaces(int nspaces) const;

  public:

    InputItem();

    // Virtual destructor needed to call delete on base class
    // http://stackoverflow.com/questions/461203/when-to-use-virtual-destructors#461224
    // (In this case, it's really just to suppress the compiler warning, since
    // InputItems don't dynamically allocate any memory.)

    virtual ~InputItem();

    // Set attributes

    void setName(const std::string & name);
    void setPosition(int y, int x);
    void setWidth(int width);
    void setWindow(WINDOW *win);
    void setAutoPosition(bool auto_position);

    // Get attributes

    const std::string & name() const;
    int posx() const;
    int posy() const;
    int width() const;
    bool selectable() const;
    bool autoPosition() const;
    const std::string & itemType() const;

    // User interaction

    virtual void draw(int y_offset, bool force=false, bool highlight=false) = 0;
    virtual std::string exec(int y_offset);

    // Accessing properties of different types. Derived classes should
    // reimplement these as needed.

    virtual std::string getStringProp() const;
    virtual bool getBoolProp() const;
    virtual int getIntProp() const;
    virtual double getDoubleProp() const;
};
