#include "CTower.h"
#include "CUnit.h"
#include "CMap.h"
#include "CEnemy.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <memory>

// fire at single enemy
void CTower::fire(const CMap &map)
{
  // if has reloaded ...
  if (m_ReloadTime > 0)
  {
    m_ReloadTime--;
    return;
  }
  // ... find target and fire
  std::shared_ptr<CEnemy> target = findTarget(map);
  if (target != nullptr)
  {
    target->getHit(m_Damage);
    m_ReloadTime = m_DEFAULT_RELOAD_TIME;
  }
  m_ReloadTime--;
  return;
}

bool CTower::setValues(int *parameters)
{
  m_Symbol = parameters[0];
  m_Damage = parameters[1];
  m_DEFAULT_RELOAD_TIME = parameters[2];
  m_Range = parameters[3];
  m_Cost = parameters[4];
  m_Type = parameters[5];
  if (m_Type < 1 || m_Type > 3)
  { // we recognize only types 1/2/3
    return false;
  }
  return true;
}

std::string CTower::printValues() const
{
  std::stringstream sstream;
  // header of the table: "sym-type-dmg-cld-rng-cost"
  // so the numbers fit in the table
  int Damage = m_Damage < 99999 ? m_Damage : 99999;
  int DEFAULT_RELOAD_TIME = m_DEFAULT_RELOAD_TIME < 999 ? m_DEFAULT_RELOAD_TIME : 999;
  int Range = m_Range < 999 ? m_Range : 999;
  int Cost = m_Cost < 9999 ? m_Cost : 9999;
  sstream << m_Symbol << " ";
  sstream << std::setfill(' ') << std::setw(3) << m_Type << m_Variety << " "
          << std::setw(5) << Damage << " "
          << std::setw(3) << DEFAULT_RELOAD_TIME << " "
          << std::setw(3) << Range << " "
          << std::setw(4) << Cost;
  return sstream.str();
}

// find target that has the shortest path ahead of him
std::shared_ptr<CEnemy> CTower::findTarget(const CMap &map) const
{
  std::shared_ptr<CEnemy> target = nullptr;
  int minSteps = INT32_MAX; // some very high number, probably (and hopefully) higher than enemies' remaining steps to end
  int currSteps;
  for (int x = m_Position.m_X - m_Range; x <= m_Position.m_X + m_Range; x++)
  {
    for (int y = m_Position.m_Y - m_Range; y <= m_Position.m_Y + m_Range; y++)
    {
      std::shared_ptr<std::list<std::shared_ptr<CEnemy>>> list = map.getEnemiesAtField(x, y);
      if (list == nullptr)
      {
        continue;
      }
      for (std::shared_ptr<CEnemy> enemy : *list)
      {
        currSteps = enemy->getStepsToEnd();
        if (enemy->isAlive() && (m_Type == 3 || enemy->getType() == m_Type) && currSteps < minSteps)
        {
          minSteps = currSteps;
          target = enemy;
        }
      }
    }
  }
  return target;
}
