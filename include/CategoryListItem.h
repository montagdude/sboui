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

    // Set/get attributes

    void setTagged(bool tagged);
    bool tagged() const;

    // The following are included to satisfy the compiler only

    void setInstalled(bool installed);
    bool installed() const;
};
