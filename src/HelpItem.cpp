#include <string>
#include "ListItem.h"
#include "HelpItem.h"

/*******************************************************************************

Constructors

*******************************************************************************/
HelpItem::HelpItem() 
{ 
  _name = ""; 

  // Add props in reverse alphabetical order (should be fastest)

  addBoolProp("space", false);
  addProp("shortcut", "");
  addBoolProp("header", false);
}

HelpItem::HelpItem(const std::string & name, const std::string & shortcut,
                   bool header, bool space)
{
  _name = name; 

  // Add props in reverse alphabetical order (should be fastest)

  addBoolProp("space", space);
  addProp("shortcut", shortcut);
  addBoolProp("header", header);
}
