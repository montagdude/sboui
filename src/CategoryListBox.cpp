#include <vector>
#include <string>
#include "ListBox.h"
#include "CategoryListBox.h"

/*******************************************************************************

Constructors

*******************************************************************************/
CategoryListBox::CategoryListBox()
{
  _win = NULL;
  _name = "";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
}

CategoryListBox::CategoryListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void CategoryListBox::addCategory(CategoryListItem *item) { addItem(item); }
