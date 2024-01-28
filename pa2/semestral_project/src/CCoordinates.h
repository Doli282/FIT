#ifndef CCOORDINATES_H
#define CCOORDINATES_H

#include <string>

/**
 * @brief Class representing coordinates
 *  m_X = x axis
 *  m_Y = y axis
 */
struct CCoordinates
{
public:
  /**
   * @brief Construct a new CCoordinates object
   *
   */
  CCoordinates() {}
  /**
   * @brief Construct a new CCoordinates object
   *
   * @param x [in] x coordinate
   * @param y [in] y coordinate
   */
  CCoordinates(int x, int y) : m_X(x), m_Y(y){};

  /**
   * @brief check if coordinates are in given field
   * -> m_X >= 0 && m_X < x && m_Y >= 0 && m_Y < y
   * @param x width of the field
   * @param y height of the field
   * @return true - is in the field
   * @return false - is outside the field
   */
  bool isInField(int x, int y) const
  {
    if (m_X >= 0 && m_X < x && m_Y >= 0 && m_Y < y)
    {
      return true;
    }
    return false;
  }

  /**
   * @brief Print coordinates
   *
   * @return std::string = m_X + " " + m_Y
   */
  std::string print() const
  {
    return (std::to_string(m_X) + " " + std::to_string(m_Y));
  }

  bool operator==(const CCoordinates &other) const
  {
    return (m_X == other.m_X && m_Y == other.m_Y);
  }
  bool operator!=(const CCoordinates &other) const
  {
    return !(*this == other);
  }
  bool operator<(const CCoordinates &other) const
  {
    if ((m_X < other.m_X) || (m_X == other.m_X && m_Y < other.m_Y))
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  /** x axis */
  int m_X;
  /** y axis */
  int m_Y;
};

#endif