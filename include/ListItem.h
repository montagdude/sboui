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
    void setName(const std::string & name);
    const std::string & name() const;
};
