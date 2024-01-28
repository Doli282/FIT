#ifndef CUNIT_H
#define CUNIT_H

#include "CCoordinates.h"

#include <memory>
#include <string>

/**
 * @brief  Class wrapping up all interacting objects in the game
 *  Has the property m_Symbol and m_Position
 */
class CUnit
{
public:
  /**
   * @brief Construct a new CUnit object
   *
   * @param symbol [in] symbol of the unit
   */
  CUnit(char symbol) : m_Symbol(symbol) {}

  /**
   * @brief Construct a new CUnit object
   *
   * @param position [in] spawning position of the unit
   * @param symbol [in] symbol of the unit
   */
  CUnit(CCoordinates position, char symbol) : m_Position(position), m_Symbol(symbol) {}

  /**
   * @brief Print parameters of given unit
   *
   * @return std::string parameters of the unit
   */
  virtual std::string print() const
  {
    std::string s;
    s.push_back(m_Symbol);
    return (s + " " + m_Position.print());
  }

  /**
   * @brief Get the m_Position object
   *
   * @return CCoordinates - current position
   */
  CCoordinates getPosition() const { return m_Position; }

  /**
   * @brief Set the m_Position object
   *
   * @param position [in] new assigned position
   */
  void setPosition(CCoordinates position)
  {
    m_Position = position;
    return;
  }

  /**
   * @brief Get the m_Symbol object
   *
   * @return char - m_Symbol
   */
  char getSymbol() const { return m_Symbol; }

  /**
   * @brief Set the m_Symbol object
   *
   * @param symbol [in] new assigned symbol
   */
  void setSymbol(char symbol)
  {
    m_Symbol = symbol;
    return;
  }

protected:
  /** position in the map */
  CCoordinates m_Position;
  /** graphic representation */
  char m_Symbol;
};

#endif /* CUNIT_H */