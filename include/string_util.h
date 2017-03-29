#pragma once

#include <string>
#include <vector>

extern std::string int_to_string(int inval);
extern std::string chararray_to_string(char chararray[]);
extern std::string string_to_lower(const std::string & instr);
extern std::string trim(const std::string & instr);
extern std::string remove_leading_whitespace(const std::string & instr);
extern std::string remove_comment(const std::string & instr,
                                  char comment='#');
extern std::vector<std::string> split(const std::string & instr,
                                      char delim=' ');
extern std::vector<std::string> wrap_words(const std::string & instr,
                                           unsigned int width);
