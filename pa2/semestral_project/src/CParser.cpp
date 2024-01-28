#include "CParser.h"

#include <iostream>
#include <cassert>
#include <string>

CParser::CParser()
{
  // test if methods are working properly
  asserts();
}

// check if filename has desired prefix (case insensitive)
bool CParser::nameMatch(const std::string &filename, const std::string &prefix) const
{
  unsigned int prefixLength = prefix.length();
  if (prefixLength > filename.length())
  {
    return false;
  }
  for (unsigned int i = 0; i < prefixLength; i++)
  {
    if (tolower(prefix.at(i)) != tolower(filename.at(i)))
    {
      return false;
    }
  }
  return true;
}

// check if input string is a valid number (positive integer)
bool CParser::parseNumber(int &parameter, const std::string &input)
{
  try
  {
    // empty string
    if (input.empty())
    {
      throw std::invalid_argument("empty string");
    }

    // string contains not-a-number characters -> catches negative numbers, too
    if (input.find_first_not_of("0123456789") != std::string::npos)
    {
      throw std::invalid_argument("not a number");
    }

    // string starts with zero and it is not only zero
    if (input.at(0) == '0' && input[1] != '\0')
    {
      throw std::invalid_argument("begins with 0");
    }

    // finally parse string to int
    parameter = std::stoi(input);
  }
  catch (...)
  {
    return false;
  }
  return true;
}

// check if other methods are working preperly
bool CParser::asserts()
{
  int x;
  assert(parseNumber(x, "101"));
  assert(parseNumber(x, "0"));
  assert(!parseNumber(x, "-101"));
  assert(!parseNumber(x, "0101"));
  assert(!parseNumber(x, "10.1"));
  assert(!parseNumber(x, "10,1"));
  assert(!parseNumber(x, "101a"));
  assert(!parseNumber(x, "a101"));
  assert(!parseNumber(x, "10b1"));
  assert(!parseNumber(x, "a"));

  std::string prefix = "CTower_";
  assert(nameMatch("CTower_a.txt", prefix));
  assert(nameMatch("CTower_", prefix));
  assert(nameMatch("cToWeR_a", prefix));
  assert(!nameMatch("aCTower_a.txt", prefix));
  assert(!nameMatch("CTow", prefix));
  assert(!nameMatch("Tower", prefix));
  return true;
}