#pragma once

#include <string>
#include "Color.h"

/* Stores color settings */

namespace color 
{
  extern Color colors;
  extern std::string fg_normal, bg_normal;
  extern std::string fg_title, bg_title;
  extern std::string fg_info, bg_info;
  extern std::string fg_highlight_active, bg_highlight_active;
  extern std::string fg_highlight_inactive, bg_highlight_inactive;
  extern std::string header;
  extern std::string tagged;
}
