#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cctype>  // isdigit
#include "string_util.h"

/*******************************************************************************

Converts an integer to string

*******************************************************************************/
std::string int_to_string(int inval)
{
  std::string outstr;
  std::stringstream ss;

  ss << inval;
  ss >> outstr;
  return outstr;
}

/*******************************************************************************

Checks whether a string can be converted to an integer

*******************************************************************************/
bool is_integer(const std::string & instr)
{
  unsigned int len, i;

  len = instr.length();
  for ( i = 0; i < len; i++ )
  {
    if ( ! std::isdigit(instr[i]) ) { return false; }
  }
  return true;
}

/*******************************************************************************

Converts a char array to string

*******************************************************************************/
std::string chararray_to_string(char chararray[])
{
  std::string outstr;
  std::stringstream ss;

  ss << chararray;
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

Removes leading whitespace

*******************************************************************************/
std::string remove_leading_whitespace(const std::string & instr)
{
  unsigned int i, trimlen, len;

  len = instr.size();
  trimlen = 0;
  for ( i = 0; i < len; i++ )
  {
    if (instr[i] == ' ') { trimlen += 1; }
    else { break; }
  }

  return instr.substr(trimlen, len-trimlen);
}

/*******************************************************************************

Removes trailing comment from string

*******************************************************************************/
std::string remove_comment(const std::string & instr, char delim)
{
  std::size_t comment_pos;

  comment_pos = instr.find_first_of(delim);
  if (comment_pos != std::string::npos)
    return instr.substr(0, comment_pos);
  else
    return instr;
}

/*******************************************************************************

Splits a string into a vector of strings. Adapted from the answer here:
http://stackoverflow.com/questions/236129/split-a-string-in-c#236803

*******************************************************************************/
std::vector<std::string> split(const std::string & instr, char delim)
{
  std::stringstream ss(instr);
  std::vector<std::string> splitstr;
  std::string item;

  while (std::getline(ss, item, delim)) { splitstr.push_back(trim(item)); }

  return splitstr;
}

/*******************************************************************************

Wraps words in a string into separate lines, returned as a vector of strings

*******************************************************************************/
std::vector<std::string> wrap_words(const std::string & instr,
                                    unsigned int width)
{
  std::vector<std::string> wordvec, splitstr1, splitstr2;
  unsigned int nwords, npars, i, j;
  std::string line;

  splitstr1 = split(instr, '\n');
  npars = splitstr1.size();
  for ( i = 0; i < npars; i++ )
  {
    if (splitstr1[i] == "")
    {
      wordvec.push_back("");
      continue;
    }
    splitstr2 = split(splitstr1[i]);
    nwords = splitstr2.size();
    line = splitstr2[0];
    for ( j = 1; j < nwords; j++ )
    { 
      if (line.size() + 1 + splitstr2[j].size() > width)
      {
        wordvec.push_back(line);
        line = splitstr2[j];
      }
      else { line += " " + splitstr2[j]; }
    }
    wordvec.push_back(line);
  }

  return wordvec;
}

/*******************************************************************************

Searches in a file for a pattern. Returns true if found, or false if not found
or file can't be read.

*******************************************************************************/
bool find_in_file(const std::string & pattern, const std::string & filename,
                  bool whole_word, bool case_sensitive)
{
  std::ifstream file;
  std::string searchpattern, line;
  std::vector<std::string> splitline;
  bool match;
  std::size_t pos;
  unsigned int i, nwords, wordlen;
  char last;

  file.open(filename.c_str());
  if (not file.is_open()) { return false; }

  if (case_sensitive) { searchpattern = pattern; }
  else { searchpattern = string_to_lower(pattern); }

  match = false;
  while (! file.eof())
  {
    std::getline(file, line);

    // Skip comments

    if (line[0] == '#') { continue; }

    if (! case_sensitive) { line = string_to_lower(line); }

    // Search for pattern in entire line

    if (! whole_word) 
    {
      pos = line.find(searchpattern);
      if (pos != std::string::npos) { match = true; }
    }
    
    // Search individual words

    else
    {
      splitline = split(line, ' ');
      nwords = splitline.size();
      for ( i = 0; i < nwords; i++ )
      {
        if (splitline[i] == searchpattern) { match = true; }
        else
        {
          wordlen = splitline[i].size();
          if (splitline[i].substr(0,wordlen-1) == searchpattern)
          {
            // Check for punctuation

            last = splitline[i][wordlen-1];
            if ( (last == '.') || (last == ',') || (last == ';') ||
                 (last == ':') || (last == ')') || (last == '?') ||
                 (last == '!') ) { match = true; }
          }
        }
        if (match) { break; }
      }
    }
    if (match) { break; }
  }

  file.close();
  return match;
}
