#ifndef CENEMY_H
#define CENEMY_H

#include "CUnit.h"
#include "CMap.h"
#include "CCoordinates.h"

#include <string>

class CMap;

/**
 * @brief General Abstract class for enemy troops
 *
 */
class CEnemy : public CUnit
{
public:
  /**
   * @brief Construct a new CEnemy object
   *
   * @param symbol [in] symbol of the unit
   */
  CEnemy(char symbol) : CUnit(symbol) {}

  /**
   * @brief Construct a new CEnemy object
   *
   * @param position [in] spawning position
   * @param symbol [in] symbol of the unit
   */
  CEnemy(CCoordinates position, char symbol) : CUnit(position, symbol) {}

  /**
   * @brief Make another instance of this class
   *
   * @return std::shared_ptr<CEnemy>
   */
  virtual std::shared_ptr<CEnemy> clone() const = 0;

  /**
   * @brief Move to the next field
   * Abstract for CEnemy
   * @return true - successfully moved
   * @return false - end reached
   */
  virtual bool movement() = 0;

  /**
   * @brief Precalculate path for every Unit of the same class
   * Abstract for CEnemy
   * @param map [in] map through which it moves
   * @return true - path found
   * @return false - path was not found
   */
  virtual bool calculatePath(const CMap &map) = 0;

  /**
   * @brief Set path to unprepared state
   *
   */
  virtual void resetPath() = 0;

  /**
   * @brief Smart enemies search for the end proactively
   *
   * @return true - search for the end actively
   * @return false -  find the end by chance eventually
   */
  virtual bool isSmart() const = 0;

  /**
   * @brief Get the StepsToEnd object
   *
   * @return size_t how many steps are remaining
   */
  size_t getStepsToEnd() const { return m_StepsToEnd; }

  /**
   * @brief inflict damage to the unit
   *
   * @param damage [in] how much should the HP decrease
   */
  virtual void getHit(int damage) { m_HP -= damage; }

  /**
   * @brief Set the parameters of the unit
   *
   * @param parameters [in] array of 4 ints
   * @return true - success
   * @return false - invalid parameters (invald type)
   */
  bool setValues(int *parameters);

  /**
   * @brief Print parametes in an organized manner
   * header of the table "sym-HP-path-type-pts"
   *
   * @return std::string
   */
  virtual std::string printValues() const;

  /**
   * @brief Get the Type object
   *
   * @return int
   */
  int getType() const { return m_Type; }

  /**
   * @brief Get the Points object
   *
   * @return int
   */
  int getPoints() const { return m_Points; }

  /**
   * @brief Check if enemy is alive (HP > 0)
   *
   * @return true - alive
   * @return false - dead
   */
  bool isAlive() const { return m_HP > 0; }

  /**
   * @brief Check if unit reached end
   *
   * @return true - reached end
   * @return false - still on its way
   */
  bool reachedEnd() const { return m_ReachedEnd; }

protected:
  /** current health points of the unit */
  int m_HP = 1;
  /** type of the unit -> 1 ground, 2-> air */
  int m_Type = 1;
  /** reward for killing this unit */
  int m_Points = 10;
  /** current position in the path */
  size_t m_PositionInPath = 0;
  /** flag if the unit has reached end */
  bool m_ReachedEnd = false;
  /** steps remianing to the end */
  size_t m_StepsToEnd = INT32_MAX;
  /** symbol identifying how this units moves */
  std::string m_PathSymbol = "bas";
};

#endif