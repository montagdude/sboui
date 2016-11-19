#pragma once

#include <string>

/*******************************************************************************

Basic class for an item that goes in a list box. Derived classes based on this
will have more members.

*******************************************************************************/
class ListItem {

  protected:

    std::string _name;

  public:

    ListItem();

    // Set attributes

    virtual void setName(const std::string & name);
    virtual void setTagged(bool tagged) = 0;

    // Get properties

    virtual const std::string & name() const;
    virtual bool tagged() const = 0;
};
