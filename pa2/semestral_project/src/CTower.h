#ifndef CTOWER_H
#define CTOWER_H

#include "CUnit.h"
#include "CEnemy.h"
#include "CMap.h"

#include <string>
#include <memory>
class CEnemy;
class CMap;

/**
 * @brief General class for towers
 *
 */
class CTower : public CUnit
{
public:
  /**
   * @brief Construct a new CTower object
   *
   * @param symbol [in] symbol of the unit
   */
  CTower(char symbol) : CUnit(symbol) {}

  /**
   * @brief Create pointer to new instance of the tower
   *
   * @return std::shared_ptr<CTower>
   */
  virtual std::shared_ptr<CTower> clone() const { return std::make_shared<CTower>(*this); }

  /**
   * @brief Fire at the chosen target
   *
   * @param map [in] map of the surrounding environment
   */
  virtual void fire(const CMap &map);

  /**
   * @brief set values to paramaters of CTower
   *
   * @param parameters [in] array of five ints
   * @return true - settings completed
   * @return false - unused
   */
  bool setValues(int *parameters);

  /**
   * @brief print parameters of the object in organized manner
   *  header of the table: "sym-type-dmg-cld-rng-cost"
   * @return std::string with all parameters excluding position
   */
  std::string printValues() const;

  /**
   * @brief Get the Cost object
   *
   * @return int
   */
  int getCost() const { return m_Cost; }

protected:
  /** damage dealt to enemy unit with one hit */
  int m_Damage;
  /** fires one shot every n turns */
  int m_DEFAULT_RELOAD_TIME;
  /** turns before next shot */
  int m_ReloadTime = 0;
  /** how many fields away can tower fire */
  int m_Range;
  /** building cost of the unit */
  int m_Cost;
  /** Determine if ground (1) air (2) or both (3) can be reached */
  int m_Type;
  /** Determine what kind of tower this is */
  char m_Variety = 'B';

  /**
   * @brief Finds the most suitable target according to its remaining steps to the goal
   *
   * @param map [in] map of the surrounding environment
   * @return std::shared_ptr<CEnemy>
   */
  std::shared_ptr<CEnemy> findTarget(const CMap &map) const;
};

#endif