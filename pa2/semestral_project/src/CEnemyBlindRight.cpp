#include "CEnemyBlindRight.h"
#include "CCoordinates.h"
#include "CMap.h"

#include <vector>

std::vector<CCoordinates> CEnemyBlindRight::m_Path;
bool CEnemyBlindRight::m_PathPrepared = false;
size_t CEnemyBlindRight::m_PathLength;

bool CEnemyBlindRight::movement()
{
  if (!m_PathPrepared)
  {
    return true;
  }
  // end reached
  if (m_PositionInPath == m_PathLength)
  {
    // sentence to death after reaching end
    m_HP = 0;
    m_ReachedEnd = true;
    return false;
  }
  // move to the next position;
  m_Position = m_Path.at(m_PositionInPath);
  m_PositionInPath++;
  m_StepsToEnd--;
  return true;
}

// calculate path for every blind unit -> right
bool CEnemyBlindRight::calculatePath(const CMap &map)
{
  if (m_PathPrepared)
  {
    return true;
  }
  // initialize positions
  CCoordinates start = map.getStart()->getPosition();
  CCoordinates end = map.getEnd()->getPosition();
  CCoordinates currentPosition = start;

  // supporting information about the map
  int bottomBorder = map.getHeight();
  int rightBorder = map.getWidth();
  // 1 faces left, 2 up, 3 right, 4 down
  int direction = map.getStart()->getNeighbouringBorder(map.getWidth());

  // some backdoor against infinite loop
  int maxMoves = bottomBorder * rightBorder * 2;
  if (maxMoves < 0)
  { // against overflow
    maxMoves = INT32_MAX;
  }
  m_PathLength = 0;
  m_Path.clear();
  // till I reach the end
  while (currentPosition != end)
  {
    int fails = 0; // minus points for each examined blocked field
    // choose where to start from the switch statements
    switch (direction)
    {
    case0:
    // check if not at the edge and the emptiness of the adjacent field
    case 4: // faces down
      // examine field to the left
      if (currentPosition.m_X != 0 && map.isFieldEmpty(currentPosition.m_X - 1, currentPosition.m_Y))
      {
        currentPosition.m_X--;
        direction = 1;
        break;
      }
      fails++;
    case 3: // faces right
      // examine field below
      if (currentPosition.m_Y != (bottomBorder - 1) && map.isFieldEmpty(currentPosition.m_X, currentPosition.m_Y + 1))
      {
        currentPosition.m_Y++;
        direction = 4;
        break;
      }
      fails++;
    case 2: // faces up
      // the field on the right
      if (currentPosition.m_X != (rightBorder - 1) && map.isFieldEmpty(currentPosition.m_X + 1, currentPosition.m_Y))
      {
        currentPosition.m_X++;
        direction = 3;
        break;
      }
      fails++;
    case 1: // faces left
      // exanime field above me
      if (currentPosition.m_Y != 0 && (map.isFieldEmpty(currentPosition.m_X, currentPosition.m_Y - 1)))
      {
        // if it is empty, move there
        currentPosition.m_Y--;
        direction = 2;
        break;
      }
      fails++;
    default:
      if (fails >= 4)
      { // unable to find move at all
        return false;
      }
      // continue in the circle
      goto case0;
    }

    // insert into path and increase the calculator of moves
    m_Path.push_back(currentPosition);
    m_PathLength++;

    // maximal number of moves reached - do not know the way out
    if (m_PathLength > maxMoves)
    {
      return false;
    }
  }
  m_StepsToEnd = m_PathLength;
  m_PathPrepared = true;
  return true;
}