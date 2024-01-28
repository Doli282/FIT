#include "CTowerSplash.h"
#include "CMap.h"

#include <memory>
#include <list>

// deal damage to all enemies in range
void CTowerSplash::fire(const CMap &map)
{
  if (m_ReloadTime > 0)
  { // still reloading
    m_ReloadTime--;
    return;
  }
  // attack all enemies in range
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
        if (enemy->isAlive() && (m_Type == 3 || enemy->getType() == m_Type))
        {
          enemy->getHit(m_Damage);
          m_ReloadTime = m_DEFAULT_RELOAD_TIME;
        }
      }
    }
  }
  m_ReloadTime--;
}