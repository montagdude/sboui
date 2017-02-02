#pragma once

#include <string>
#include <vector>

extern std::string int2string(int inval);
extern std::string string_to_lower(const std::string & instr);
extern std::string trim(const std::string & instr);
extern std::vector<std::string> wrap_words(const std::string & instr,
                                           unsigned int width);
