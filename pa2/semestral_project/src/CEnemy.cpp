#include "CEnemy.h"


#include <sstream>
#include <iomanip>

bool CEnemy::setValues(int *parameters)
{
  m_Symbol = parameters[0];
  m_Type = parameters[1];
  if (m_Type != 1 && m_Type != 2)
  {
    return false;
  }
  m_HP = parameters[2];
  m_Points = parameters[3];
  return true;
}

std::string CEnemy::printValues() const
{
  std::stringstream sstream;
  int HP = m_HP < 9999 ? m_HP : 9999;
  int points = m_Points < 999999 ? m_Points : 999999;
  sstream << m_Symbol << " ";
  sstream << std::setfill(' ') << std::setw(4) << HP << " "
          << std::setw(3) << m_PathSymbol << " "
          << std::setw(2) << m_Type << " "
          << std::setw(6) << points;
  return sstream.str();
}