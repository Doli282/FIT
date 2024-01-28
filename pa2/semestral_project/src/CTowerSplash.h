#ifndef CTOWERSPLASH_H
#define CTOWERSPLASH_H

#include "CTower.h"
#include "CMap.h"

#include <memory>

/**
 * @brief Deals splash damage in an area around itself
 *
 */
class CTowerSplash : public CTower
{
public:
  /**
   * @brief Construct a new CTowerSplash object
   *
   * @param symbol [in] symbol of the unit
   */
  CTowerSplash(char symbol) : CTower(symbol)
  {
    m_Variety = 'S';
  }

  /**
   * @brief Create pointer to new instance of the tower
   *
   * @return std::shared_ptr<CTower>
   */
  std::shared_ptr<CTower> clone() const override { return std::make_shared<CTowerSplash>(*this); }

  /**
   * @brief Fire at the chosen target
   *
   * @param map [in] map of the surrounding environment
   */
  void fire(const CMap &map) override;
};

#endif