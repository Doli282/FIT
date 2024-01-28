#include "CEnemyBlindLeft.h"
#include "CCoordinates.h"

#include <vector>

std::vector<CCoordinates> CEnemyBlindLeft::m_Path;
bool CEnemyBlindLeft::m_PathPrepared = false;
size_t CEnemyBlindLeft::m_PathLength;

// move to the next field
bool CEnemyBlindLeft::movement()
{
  if (!m_PathPrepared)
  { // stop movement if path has not been prepared
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
bool CEnemyBlindLeft::calculatePath(const CMap &map)
{
  if (m_PathPrepared)
  {
    return true;
  }
  // initialize positions
  CCoordinates end = map.getEnd()->getPosition();
  CCoordinates currentPosition = map.getStart()->getPosition();

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
  // std::cout << "there" << std::endl;
  // std::cout << "curr: " << currentPosition.m_X << "x" << currentPosition.m_Y << std::endl;
  // std::cout << "end" << end.m_X << "x" << end.m_Y << std::endl;
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
    case 1: // faces left
            // examine field below
      if (currentPosition.m_Y != (bottomBorder - 1) && map.isFieldEmpty(currentPosition.m_X, currentPosition.m_Y + 1))
      {
        currentPosition.m_Y++;
        direction = 4;
        break;
      }
      fails++;
    case 2: // faces up
      // examine field to the left
      if (currentPosition.m_X != 0 && map.isFieldEmpty(currentPosition.m_X - 1, currentPosition.m_Y))
      {
        currentPosition.m_X--;
        direction = 1;
        break;
      }
      fails++;
    case 3: // faces right
      // exanime field above me
      if (currentPosition.m_Y != 0 && (map.isFieldEmpty(currentPosition.m_X, currentPosition.m_Y - 1)))
      {
        // if it is empty, move there
        currentPosition.m_Y--;
        direction = 2;
        break;
      }
      fails++;
    case 4: // faces down
      // the field on the right
      if (currentPosition.m_X != (rightBorder - 1) && map.isFieldEmpty(currentPosition.m_X + 1, currentPosition.m_Y))
      {
        currentPosition.m_X++;
        direction = 3;
        break;
      }
      fails++;

    default:
      if (fails >= 4)
      { // unable to find move at all
        return false;
      }
      goto case0;
    }

    // insert into path and increase the calculator of moves
    m_Path.push_back(currentPosition);
    // std::cout << "curr: " << currentPosition.m_X << "x" << currentPosition.m_Y << std::endl;
    m_PathLength++;

    // maximal number of moves reached - do not know the way out
    if (m_PathLength > maxMoves)
    {
      return false;
    }
  }
  m_StepsToEnd = m_PathLength;
  m_PathPrepared = true;
  // std::cout << "---------" << m_PathLength << std::endl;
  return true;
}