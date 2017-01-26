#include <string>
#include <sstream>

std::string int2string(int inval)
{
  std::string outstr;
  std::stringstream ss;

  ss << inval;
  ss >> outstr;
  return outstr;
}

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
