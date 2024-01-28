#ifndef CENEMYSHORTESTAIR_H
#define CENEMYSHORTESTAIR_H

#include "CEnemy.h"
#include "CMap.h"
#include "CCoordinates.h"

#include <vector>

/**
 * @brief CEnemy class for units following the shortest path through the air
 *
 */
class CEnemyShortestAir : public CEnemy
{
public:
  /**
   * @brief Construct a new CEnemyShortestAir object
   *
   * @param symbol [in] symbol of the unit
   */
  CEnemyShortestAir(char symbol) : CEnemy(symbol)
  {
    m_Type = 2;
    m_PathSymbol = "AIR";
  }

  /**
   * @brief Construct a new CEnemyShortestAir object
   *
   * @param position [in] spawning position
   * @param symbol [in] symbol of the unit
   */
  CEnemyShortestAir(CCoordinates position, char symbol) : CEnemy(position, symbol)
  {
    m_Type = 2;
    m_PathSymbol = "AIR";
  }

  /**
   * @brief Make another instance of this class
   *
   * @return std::shared_ptr<CEnemy>
   */
  std::shared_ptr<CEnemy> clone() const override { return std::make_shared<CEnemyShortestAir>(*this); }

  /**
   * @brief Move to the next field
   *
   * @return true - successfully moved
   * @return false - end reached
   */
  bool movement() override;

  /**
   * @brief Precalculate path for every Unit of the same class
   *
   * @param map [in] map through which it moves
   * @return true - path found
   * @return false - path was not found
   */
  bool calculatePath(const CMap &map) override;

  /**
   * @brief Reset path to unprepared form - clear it
   *
   */
  void resetPath() override { m_PathPrepared = false; }

  /**
   * @brief Smart enemies search for the end proactively
   *
   * @return true - search for the end actively
   * @return false -  find the end by chance eventually
   */
  bool isSmart() const override { return true; }

protected:
  /** vector with positions defining the path from start (excluding) to end (including) */
  static std::vector<CCoordinates> m_Path;
  /** flag if path has been prepared */
  static bool m_PathPrepared;
  /** length of the path */
  static size_t m_PathLength;
};

#endif