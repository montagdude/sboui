#pragma once

#include "ListItem.h"

/*******************************************************************************

List item for use in Category list boxes

*******************************************************************************/
class CategoryListItem: public ListItem {

  private:

    bool _tagged;

  public:

    // Constructor

    CategoryListItem();

    // Set/get properties

    void setTagged(bool tagged);
    bool tagged() const;
};
