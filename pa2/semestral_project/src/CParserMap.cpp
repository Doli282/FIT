#include "CParserMap.h"
#include "CTowerWiki.h"
#include "CCoordinates.h"
#include "CTower.h"

#include <string>
#include <memory>

// parse coordinates of a filed in the map
bool CParserMap::parsePoint(CCoordinates &coordinates, const std::string &axisX, const std::string &axisY, int width, int height)
{
  // parse cooridnates of individual axes
  if (!parseNumber(coordinates.m_X, axisX))
  {
    return false;
  }
  if (!parseNumber(coordinates.m_Y, axisY))
  {
    return false;
  }

  // check if coordinates are not outside the range
  if (!coordinates.isInField(width, height))
  { // coordinates are too big
    return false;
  }
  /*
  // is the point unoccupied?
  if (atField(coordinates) != nullptr)
  {
    return false;
  }
  */
  return true;
}

// parse coordinates of Start and End
bool CParserMap::parseDistinctPoint(CCoordinates &coordinates, const std::string &axisX, const std::string &axisY, int width, int height)
{
  // parse coordinates
  if (!parsePoint(coordinates, axisX, axisY, width, height))
  {
    return false;
  }
  // starting/ending position has to be at the edge
  if (coordinates.m_X != 0 && coordinates.m_X != (width - 1) && coordinates.m_Y != 0 && coordinates.m_Y != (height - 1))
  {
    return false;
  }
  // starting/ending point can be in the corner
  return true;
}

// get CTower from its symbol and coordinates
bool CParserMap::parseCTower(const std::string &symbol, std::shared_ptr<CTower> &pointer,
                             CCoordinates &coordinates, const std::string &axisX, const std::string &axisY, int width, int height)
{
  // parse coordinates
  if (!parsePoint(coordinates, axisX, axisY, width, height))
  {
    return false;
  }
  // there is only one symbol
  if (symbol.length() != 1)
  {
    return false;
  }
  char symbolCH = symbol.at(0);
  // the symbol is graphical but is not a number
  if (!isgraph(symbolCH) || isdigit(symbolCH))
  {
    return false;
  }
  // check if it is a symbol for known CTower
  CTowerWiki wiki;
  if (!wiki.isInWiki(symbolCH))
  {
    return false;
  }
  // assign the pointer and if it is not a null pointer create a copy
  pointer = wiki.getCTower(symbolCH);
  if (pointer != nullptr)
  {
    pointer = pointer->clone();
  }
  return true;
}