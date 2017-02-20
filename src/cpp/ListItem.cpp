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
int ListItem::propIdxByName(const std::string & propname) const
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

Constructors

*******************************************************************************/
ListItem::ListItem() 
{ 
  _name = ""; 
  _props.resize(0);
  _hotkey = -1;
}

ListItem::ListItem(const std::string & name) 
{ 
  _name = name; 
  _props.resize(0);
  _hotkey = -1;
}

/*******************************************************************************

Set properties. Methods with return value return 0 for success or 1 for failure.

*******************************************************************************/
void ListItem::setName(const std::string & name) { _name = name; }
void ListItem::setHotKey(int hotkey) { _hotkey = hotkey; }
void ListItem::addProp(const std::string & propname, const std::string & value)
{
  int propidx;
  listprop prop;

  propidx = propIdxByName(propname);
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

  propidx = propIdxByName(propname);
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

  propidx = propIdxByName(propname);
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

  propidx = propIdxByName(propname);
  if (propidx == -1) { return 1; }
  else
  {
    setPropByIdx(propidx, bool2String(value));
    return 0;
  }
}

/*******************************************************************************

Access properties. Warning: getProp methods do not check whether the prop is
actually present, so attempting to access invalid props will result in a
segfault. If you are not sure whether the prop is present, check first with
checkProp.

*******************************************************************************/
const std::string & ListItem::name() const { return _name; }
int ListItem::hotKey() const { return _hotkey; }
bool ListItem::checkProp(const std::string & propname) const
{
  if ( (propname == "name") || (propIdxByName(propname) != -1) )
    return true;
  else { return false; }
}

const std::string & ListItem::getProp(const std::string & propname) const
{
  int propidx;
  
  if (propname == "name") { return _name; }
  else
  {
    propidx = propIdxByName(propname);
    return _props[propidx].value;
  }
}

bool ListItem::getBoolProp(const std::string & propname) const
{
  int propidx;
  
  propidx = propIdxByName(propname);
  return string2Bool(_props[propidx].value);
}
