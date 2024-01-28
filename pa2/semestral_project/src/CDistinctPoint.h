#ifndef CDISTINCTPOINT_H
#define CDISTINCTPOINT_H

#include "CUnit.h"
#include "CCoordinates.h"

/**
 * @brief Class for START and END
 * 
 */
class CDistinctPoint : public CUnit
{
public:
  /**
   * @brief Construct a new CDistinctPoint object
   * 
   * @param position [in] spwaning coordinates
   * @param symbol [in] symbol of the unit
   */
  CDistinctPoint(CCoordinates position, char symbol) : CUnit(position, symbol) {}

  /**
   * @brief set symbol of the distinct point according to its position
   * end points outward, start points inward
   * @param width [in] width of the map
   * @param start [in] is this the starting point?
   */
  void specifySymbol(int width, bool start)
  {
    int border = getNeighbouringBorder(width);
    switch (border)
    {
    case 1: // right border
      m_Symbol = (start ? '<' : '>');
      break;
    case 2: // bottom border
      m_Symbol = (start ? '^' : 'V');
      break;
    case 3: // left border
      m_Symbol = (start ? '>' : '<');
      break;
    case 4: // top border
      m_Symbol = (start ? 'V' : '^');
      break;
    default:
      break;
    }
  }

  /**
   * @brief Determine near what wall is the Distinct point
   * map: corners are right or left
   * <^^>
   * <vv>
   * @param width [in] width of the map
   * @return int: 1 = right, 2 = bottom, 3 = left, 4 = top
   */
  int getNeighbouringBorder(int width) const
  {
    if (m_Position.m_X == 0)
    {
      return 3;
    }
    else if (m_Position.m_X == width - 1)
    {
      return 1;
    }
    else if (m_Position.m_Y == 0)
    {
      return 4;
    }
    else
    {
      return 2;
    }
  }

  /**
   * @brief print parameters of given distinct point
   * 
   * @return std::string -> print position
   */
  std::string print() const override
  {
    return m_Position.print();
  }
};

#endif