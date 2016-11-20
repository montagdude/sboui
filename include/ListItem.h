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
    virtual void setCategory(const std::string & category) = 0;
    virtual void setTagged(bool tagged) = 0;
    virtual void setInstalled(bool installed) = 0;

    // Get properties

    virtual const std::string & name() const;
    virtual const std::string & category() const = 0;
    virtual bool tagged() const = 0;
    virtual bool installed() const = 0;
};
