#pragma once

#include <vector>
#include <string>

/*******************************************************************************

Basic class for an item that goes in a list box. Each item can have multiple
properties and values, but it must at least have a name. Additional properties
are stored as strings, but helper functions are included to set and get them via
other types; e.g., addBoolProp, setBoolProp, and getBoolProp.

*******************************************************************************/
class ListItem {

  protected:

    struct listprop {
      std::string propname;
      std::string value;
    };

    std::string _name;
    std::vector<listprop> _props;
    int _hotkey;

    // Conversions

    std::string bool2String(bool value) const;
    bool string2Bool(const std::string & value) const;

    // Accessing props

    int propByName(const std::string & propname) const;
    void setPropByIdx(unsigned int idx, const std::string & value);

  public:

    ListItem();
    ListItem(const std::string & name);

    // Set properties

    void setName(const std::string & name);

    /* HotKey can be used to bold or underline a certain character in the name,
       and the caller can use it to perform a certain action when that keystroke
       is returned. By default it is set to -1. */
    void setHotKey(int hotkey); 
                                   
    void addProp(const std::string & propname, const std::string & value);
    void addBoolProp(const std::string & propname, bool value);
    int setProp(const std::string & propname, const std::string & value);
    int setBoolProp(const std::string & propname, bool value);

    // Get properties

    const std::string & name() const;
    int hotKey() const;
    const std::string & getProp(const std::string & propname) const;
    bool getBoolProp(const std::string & propname) const;
};
