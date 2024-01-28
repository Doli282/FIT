#include "CEnemyShortestAir.h"
#include "CCoordinates.h"
#include "CMap.h"

#include <vector>

std::vector<CCoordinates> CEnemyShortestAir::m_Path;
bool CEnemyShortestAir::m_PathPrepared = false;
size_t CEnemyShortestAir::m_PathLength;

bool CEnemyShortestAir::movement()
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

bool CEnemyShortestAir::calculatePath(const CMap &map)
{
  // initialize positions
  if (m_PathPrepared)
  {
    return true;
  }
  CCoordinates end = map.getEnd()->getPosition();
  CCoordinates currentPosition = map.getStart()->getPosition();
  m_Path.clear();
  m_PathLength = 0;

  // get closer to the end
  while (currentPosition != end)
  {
    if (currentPosition.m_X < end.m_X)
    {
      currentPosition.m_X++;
    }
    else if (currentPosition.m_X > end.m_X)
    {
      currentPosition.m_X--;
    }
    else if (currentPosition.m_Y < end.m_Y)
    {
      currentPosition.m_Y++;
    }
    else // if(currentPosition.m_Y > end.m_Y)
    {
      currentPosition.m_Y--;
    }
    m_Path.push_back(currentPosition);
    m_PathLength++;
  }
  m_StepsToEnd = m_PathLength;
  m_PathPrepared = true;
  return true;
}