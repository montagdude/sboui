#include <string>
#include "ListItem.h"
#include "HelpItem.h"

/*******************************************************************************

Constructors

*******************************************************************************/
HelpItem::HelpItem() 
{ 
  _name = ""; 
  addProp("shortcut", "");
  addBoolProp("header", false);
  addBoolProp("space", false);
}

HelpItem::HelpItem(const std::string & name, const std::string & shortcut,
                   bool header, bool space)
{
  _name = name; 
  addProp("shortcut", shortcut);
  addBoolProp("header", header);
  addBoolProp("space", space);
}
