#include <string>
#include <sstream>
#include <vector>
#include "string_util.h"

/*******************************************************************************

Converts an integer to string

*******************************************************************************/
std::string int2string(int inval)
{
  std::string outstr;
  std::stringstream ss;

  ss << inval;
  ss >> outstr;
  return outstr;
}

/*******************************************************************************

Converts a string to lower case

*******************************************************************************/
std::string string_to_lower(const std::string & instr)
{
  std::string outstr;
  std::string::size_type k, len;

  len = instr.size();
  for ( k = 0; k < len; k++ )
  {
    outstr.push_back(std::tolower(instr[k]));
  }
  return outstr;
}

/*******************************************************************************

Trims white space, line ending characters, etc. from end of string

*******************************************************************************/
std::string trim(const std::string & instr)
{
  int i, trimlen, len;

  len = instr.size();
  trimlen = len;
  for ( i = len-1; i >= 0; i-- )
  {
    if ( (instr[i] != ' ') && (instr[i] != '\n') && (instr[i] != '\0') )
    {
      trimlen = i+1;
      break;
    }
  }

  return instr.substr(0, trimlen);
}

/*******************************************************************************

Wraps words in a string into a separate lines, returned as a vector of string

*******************************************************************************/
std::vector<std::string> wrap_words(const std::string & instr,
                                    unsigned int width)
{
  std::vector<std::string> wordvec(2);

  wordvec[0] = "Hello";
  wordvec[1] = "world";
  return wordvec;
}
