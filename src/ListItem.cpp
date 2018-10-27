#include <string>
#include <cmath>        // floor
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

Finds prop by name using bisection search

*******************************************************************************/
int ListItem::findPropByName(const std::string & propname, int & propidx,
                             int & lbound, int & rbound) const
{
  std::string left, right, mid;
  int midbound;

  left = _props[lbound].propname;
  right = _props[rbound].propname;

  // Check if outside the bounds

  if ( (propname < left) || (propname > right) )
    return 1;

  // Check bounds for match

  if (propname == left)
  {
    propidx = lbound;
    return 0;
  }
  else if (propname == right)
  {
    propidx = rbound;
    return 0;
  }

  // Cut the list in half and try again

  midbound = std::floor(double(lbound+rbound)/2.);
  mid = _props[midbound].propname;

  if (propname <= mid)
    rbound = midbound;
  else
    lbound = midbound;

  return findPropByName(propname, propidx, lbound, rbound);
}

/*******************************************************************************

Searches for a property in the list and returns its index. If it does not
exist, returns -1.

*******************************************************************************/
int ListItem::propIdxByName(const std::string & propname) const
{
  int nprops, propidx, lbound, rbound, check;

  nprops = _props.size();
  if (nprops == 0)
    return -1;

  propidx = -1;
  lbound = 0;
  rbound = nprops-1;
  check = findPropByName(propname, propidx, lbound, rbound);

  if (check == 0)
    return propidx;
  else
    return -1;
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
  unsigned int i, nprops, propinsert;
  listprop prop;

  propidx = propIdxByName(propname);
  if (propidx != -1) { setPropByIdx(propidx, value); }
  else
  {
    prop.propname = propname;
    prop.value = value;
    nprops = _props.size();

    // Insert in sorted order

    propinsert = nprops;
    for ( i = 0; i < nprops; i++ )
    {
      if (propname < _props[i].propname)
      {
        propinsert = i;
        break;
      }
    }
    _props.insert(_props.begin()+propinsert, prop);
  }
}

void ListItem::addBoolProp(const std::string & propname, bool value)
{
  int propidx;
  unsigned int i, nprops, propinsert;
  listprop prop;

  propidx = propIdxByName(propname);
  if (propidx != -1) { setPropByIdx(propidx, bool2String(value)); }
  else
  {
    prop.propname = propname;
    prop.value = bool2String(value);
    nprops = _props.size();

    // Insert in sorted order

    propinsert = nprops;
    for ( i = 0; i < nprops; i++ )
    {
      if (propname < _props[i].propname)
      {
        propinsert = i;
        break;
      }
    }
    _props.insert(_props.begin()+propinsert, prop);
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
