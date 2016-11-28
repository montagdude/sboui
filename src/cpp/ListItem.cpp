#include <string>
#include "ListItem.h"

/*******************************************************************************

Conversions

*******************************************************************************/
std::string ListItem::bool2String(bool value) const
{
  if (value) { return "true"; }
  else { return "false"; }
}

bool ListItem::string2Bool(const std::string & value) const
{
  if (value == "true") { return true; }
  else { return false; }
}

/*******************************************************************************

Searches for a property in the list and returns its index. If it does not
exist, returns -1.

*******************************************************************************/
int ListItem::propByName(const std::string & propname) const
{
  int i, nprops, propidx;

  propidx = -1;
  nprops = _props.size();
  for ( i = 0; i < nprops; i++ )
  {
    if (_props[i].propname == propname)
    {
      propidx = i;
      break;
    }
  } 

  return propidx;
}

/*******************************************************************************

Sets property by index. Warning: does not check for list bounds.

*******************************************************************************/
void ListItem::setPropByIdx(unsigned int idx, const std::string & value)
{
  _props[idx].value = value;
}

/*******************************************************************************

Constructor

*******************************************************************************/
ListItem::ListItem() 
{ 
  _name = ""; 
  _props.resize(0);
}

/*******************************************************************************

Set properties. Methods with return value return 0 for success or 1 for failure.

*******************************************************************************/
void ListItem::setName(const std::string & name) { _name = name; }
void ListItem::addProp(const std::string & propname, const std::string & value)
{
  int propidx;
  listprop prop;

  propidx = propByName(propname);
  if (propidx != -1) { setPropByIdx(propidx, value); }
  else
  {
    prop.propname = propname;
    prop.value = value;
    _props.push_back(prop);
  }
}

void ListItem::addBoolProp(const std::string & propname, bool value)
{
  int propidx;
  listprop prop;

  propidx = propByName(propname);
  if (propidx != -1) { setPropByIdx(propidx, bool2String(value)); }
  else
  {
    prop.propname = propname;
    prop.value = bool2String(value);
    _props.push_back(prop);
  }
}

int ListItem::setProp(const std::string & propname, const std::string & value)
{
  int propidx;
  listprop prop;

  propidx = propByName(propname);
  if (propidx == -1) { return 1; }
  else
  {
    setPropByIdx(propidx, value);
    return 0;
  }
}

int ListItem::setBoolProp(const std::string & propname, bool value)
{
  int propidx;
  listprop prop;

  propidx = propByName(propname);
  if (propidx == -1) { return 1; }
  else
  {
    setPropByIdx(propidx, bool2String(value));
    return 0;
  }
}

/*******************************************************************************

Access properties. Warning: does not check for invalid propname specified.
Such an attempt will result in a segfault.

*******************************************************************************/
const std::string & ListItem::name() const { return _name; }
const std::string & ListItem::getProp(const std::string & propname) const
{
  int propidx;
  
  propidx = propByName(propname);
  return _props[propidx].value;
}

bool ListItem::getBoolProp(const std::string & propname) const
{
  int propidx;
  
  propidx = propByName(propname);
  return string2Bool(_props[propidx].value);
}
