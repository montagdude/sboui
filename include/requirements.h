#pragma once

#include <vector>
#include "BuildListItem.h"

int compute_reqs_order(const BuildListItem & build,
                       std::vector<BuildListItem> & reqlist,
                       const std::vector<BuildListItem> & slackbuilds);
void compute_inv_reqs(const BuildListItem & build,
                      std::vector<BuildListItem *> & invreqlist,
                      const std::vector<BuildListItem *> & installedlist);
