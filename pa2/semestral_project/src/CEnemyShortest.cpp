#include "CEnemyShortest.h"
#include "CCoordinates.h"

#include <vector>
#include <queue>
#include <map>
#include <algorithm>

std::vector<CCoordinates> CEnemyShortest::m_Path;
bool CEnemyShortest::m_PathPrepared = false;
size_t CEnemyShortest::m_PathLength;

bool CEnemyShortest::movement()
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
  m_StepsToEnd--;
  m_Position = m_Path.at(m_PositionInPath);
  m_PositionInPath++;
  return true;
}

// calculate path using BFS
bool CEnemyShortest::calculatePath(const CMap &map)
{
  // path has already been found
  if (m_PathPrepared)
  {
    return true;
  }
  // initialize data
  int maxX = map.getWidth();
  int maxY = map.getHeight();
  std::multimap<int, int> allFieldPairs; // from, to, to, to, to
  /* coordintes converted to int = y * maxX + x
      +|0|1|2|3 <- x ->
    y 0|0|1|2|3
      1|4|5|6|7
      2|8|9|10|11
  */
  for (int x = 0; x < maxX; x++)
  {
    for (int y = 0; y < maxY; y++)
    {
      // is the starting point empty?
      if (!map.isFieldEmpty(x, y))
      {
        continue;
      }
      // if adjacent filed is empty -> place it to the multimap
      int origin = y * maxX + x;
      if (map.isFieldEmpty(x, y - 1))
      {
        allFieldPairs.emplace(origin, origin - maxX);
      }
      if (map.isFieldEmpty(x, y + 1))
      {
        allFieldPairs.emplace(origin, origin + maxX);
      }
      if (map.isFieldEmpty(x - 1, y))
      {
        allFieldPairs.emplace(origin, origin - 1);
      }
      if (map.isFieldEmpty(x + 1, y))
      {
        allFieldPairs.emplace(origin, origin + 1);
      }
    }
  }

  // initialize structures
  std::queue<int> toVisit;
  std::map<int, int> visited; // to, from
  int end = map.getEnd()->getPosition().m_Y * maxX + map.getEnd()->getPosition().m_X;
  int start = map.getStart()->getPosition().m_Y * maxX + map.getStart()->getPosition().m_X;
  int currPos;
  visited.emplace(start, start);
  toVisit.push(start);

  // BFS
  while (1)
  {
    // is there nothign to search?
    if (toVisit.empty())
    {
      return false;
    }
    currPos = toVisit.front();
    toVisit.pop();

    if (currPos == end)
    {
      break;
    }

    auto it = allFieldPairs.lower_bound(currPos);    // first valid element
    auto itEnd = allFieldPairs.upper_bound(currPos); // first invalid element
    // for every possible destination check if it was visited. If not, visit it
    while (it != itEnd)
    {
      // if not visited before place to visit
      if (visited.find(it->second) == visited.end())
      {
        visited.emplace(it->second, currPos);
        toVisit.push(it->second);
      }
      it++;
    }
  }

  // backtrace the path
  m_Path.clear();
  m_PathLength = 0;
  while (currPos != start)
  {
    m_Path.emplace_back(currPos % maxX, currPos / maxX); // retrieve coordinates
    currPos = visited.at(currPos);
    m_PathLength++;
  }
  std::reverse(m_Path.begin(), m_Path.end());
  m_StepsToEnd = m_PathLength;
  m_PathPrepared = true;
  return true;
}
