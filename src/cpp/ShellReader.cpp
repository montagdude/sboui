#include <string>
#include <vector>
#include <fstream>
#include "string_util.h"
#include "ShellReader.h"

/*******************************************************************************

Checks variable name and removes 'varname=' if it is found

*******************************************************************************/
bool ShellReader::checkVarname(std::string & line,
                               const std::string & varname) const
{
  unsigned int len;

  line = remove_comment(line, '#');
  line = remove_leading_whitespace(line);

  len = varname.size();
  if (line.substr(0, len+1) == varname + "=")
  {
    line = line.substr(len+1, line.size()-len-1);
    return true;
  }
  else { return false; }
}

/*******************************************************************************

Reads the value of a variable given the first line. It is assumed that
checkVarname has been called first so that VARNAME= has already been removed.

*******************************************************************************/
int ShellReader::readVariable(std::string & line, std::string & value)
{
  bool readnext, endvalue;
  unsigned int len;
  char quote;
  std::size_t quote_pos;

  // Read the first line

  line = trim(line);
  len = line.size();
  readnext = false;
  if ( (line[0] == '"') || (line[0] == '\'') )
  {
    quote = line[0];                        // Get quote character
    quote_pos = line.substr(1,len-1).find_first_of(quote);
    if (quote_pos != std::string::npos)
    {
      value = line.substr(1,quote_pos);     // Value between quotes
      return 0;
    } 
    else if (line[len-1] == '\\')
    { 
      value = trim(line.substr(1,len-2));   // Line continuation
      readnext = true;
    }
    else
    {
      value = trim(line.substr(1,len-1));   // Unclosed quote: go to next line
      readnext = true;
    }
  }
  else
  {
    // Just read the thing right after the equal sign
    value = split(line)[0];
    return 0;
  }

  // Keep reading subsequent lines until the quote is closed

  if (readnext)
  {
    endvalue = false;
    while (! endvalue)
    {
      if (_file.eof())
      {
        rewind();
        return 1;
      }
      std::getline(_file, line);
      line = remove_leading_whitespace(line);
      if (line[0] == '#') { continue; }     // Comment line
      line = remove_comment(line, '#');
      line = trim(line);

      len = line.size();
      quote_pos = line.substr(1,len-1).find_first_of(quote);
      if (quote_pos != std::string::npos)   // End value
      {
        endvalue = true;
        value += " " + trim(line.substr(0,quote_pos+1));
      }
      else if (line[len-1] == '\\')         // Line continuation
      {
        value += " " + trim(line.substr(0,len-2));
      }
      else                                  // Unclosed quote: go to next line
      {
        value += " " + trim(line);
      }
    }
  }

  return 0;
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
ShellReader::ShellReader() { _file_open = false; }
ShellReader::~ShellReader() { if (_file_open) { _file.close(); } }

/*******************************************************************************

Opens or closes a file. Returns 1 on error or 0 on success.

*******************************************************************************/
int ShellReader::open(const std::string & filename)
{
  _file.open((filename).c_str());
  if (not _file.is_open()) { return 1; }
  else { _file_open = true; }

  return 0;
}

int ShellReader::close()
{
  if (! _file_open) { return 1; }
  else { _file_open = false; }

  return 0;
}

/*******************************************************************************

Reads a variable from the file and stores as string. Returns 1 on error or 0
on success.

*******************************************************************************/
int ShellReader::read(const std::string & varname, std::string & value)
{
  int check;
  bool reading;
  std::string line;

  if (! _file_open) { return 1; }

  reading = true;
  check = 0;
  while (reading)
  {
    if (_file.eof())
    {
      rewind();
      return 1;
    }

    std::getline(_file, line);
    if (checkVarname(line, varname))
    { 
      check = readVariable(line, value); 
      reading = false;
    }
  }

  return check;
}

/*******************************************************************************

Rewinds to beginning of the file. Returns 1 if the file is not open.

*******************************************************************************/
int ShellReader::rewind()
{
  if (! _file_open) { return 1; }

  _file.clear();
  _file.seekg(0);

  return 0;
}
