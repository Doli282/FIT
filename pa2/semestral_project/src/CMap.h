#ifndef CMAP_H
#define CMAP_H

#include "CUnit.h"
#include "CCoordinates.h"
#include "CEnemy.h"
#include "CDistinctPoint.h"
#include "CTower.h"

#include <string>
#include <unordered_map>
#include <list>
#include <ncurses.h>
#include <memory>
class CEnemy;
class CTower;

/**
 * @brief Takes care of the map of the game
 *
 */
class CMap
{
public:
  /**
   * @brief Construct a new CMap object
   *
   */
  CMap()
  {
    m_Counter++;
  }

  /**
   * @brief Destroy the CMap object
   *
   */
  ~CMap()
  {
    m_Counter--;
    if (m_Counter <= 0 && m_Window != nullptr)
    {
      delwin(m_Window);
      delwin(m_WinStats);
    }
  }

  /**
   * @brief refresh how the map looks like
   *
   * @return true - success
   * @return false - map has not been initialized
   */
  bool display() const;

  bool buildingRegime();

  // one turn in the game
  bool turn();

  /**
   * @brief Save Game to CMap data file
   *
   * @param path [in] path to working directory
   * @param fileName [in] name of saved file
   * @param overwrite [in] command to overwrite file
   * @return int 0 = success; 1 = file exists; 2 = error
   */
  int saveData(const std::string &path, const std::string &fileName, bool overwrite = false) const;

  /**
   * @brief initialize map -> prepare map  layout
   *
   * @param path [in] path to directory with saved games
   * @param fileName [in] chosen filename
   * @param originX [in] position of the origin of the window
   * @param originY [in] position of the origin of the window
   * @return true - success
   * @return false - file could not be loaded
   */
  bool initializeMap(const std::string &path, const std::string &fileName, int originX = 0, int originY = 0);

  /**
   * @brief Get the Enemies at examined field
   *
   * @param x [in] x coordinate of the examined field
   * @param y [in] y coordinate of the examined field
   * @return std::list<std::shared_ptr<CEnemy>>&
   */
  std::shared_ptr<std::list<std::shared_ptr<CEnemy>>> getEnemiesAtField(int x, int y) const;

  /**
   * @brief Examine if field is occupied by tower
   *
   * @param x x coordinate
   * @param y y coordinate
   * @return true - field is empty
   * @return false - field is occupied
   */
  bool isFieldEmpty(int x, int y) const;

  /**
   * @brief Examine if field is occupied by tower
   *
   * @param coordinates [in] cooridinates of the field
   * @return true - field is empty
   * @return false - field is occupied
   */
  bool isFieldEmpty(const CCoordinates &coordinates) const;

  /**
   * @brief Check if player is still alive
   * m_Lives > 0
   * @return true
   * @return false
   */
  bool stillAlive() const { return m_Lives > 0; }

  /**
   * @brief Get the m_Height object
   *
   * @return int
   */
  int getHeight() const { return m_Height; }

  /**
   * @brief Get the m_Width object
   *
   * @return int
   */
  int getWidth() const { return m_Width; }

  /**
   * @brief Get the m_Start object
   *
   * @return std::shared_ptr<CDistinctPoint>
   */
  std::shared_ptr<CDistinctPoint> getStart() const { return m_Start; }

  /**
   * @brief Get the m_End object
   *
   * @return std::shared_ptr<CDistinctPoint>
   */
  std::shared_ptr<CDistinctPoint> getEnd() const { return m_End; }

protected:
  /**
   * @brief load data from data file with saved game
   *
   * @param path address of the saved game
   * @return true - loading was successful
   * @return false - problem occured - corrupted file,...
   */
  bool loadData(const std::string &path);

  /**
   * @brief Reset database for Enemies
   */
  void resetEnemies();

  /**
   * @brief Reset database for Towers
   */
  void resetTowers();

  bool dismantleTower(int x, int y, bool reimburse = false);

  bool buildTower(char symbol, int x, int y);

  std::string minString(int value, int limit) const;

  bool displayStats() const;

  /**
   * @brief Print data of CMap class including data of CUnits in the map
   *
   * @param score
   * @param coins
   * @param lives
   * @return std::string
   */
  std::string printData() const;

  /** counter of current lives */
  static int m_Lives;
  /** counter of total score */
  static int m_Score;
  /** counter of current points to spend */
  static int m_Coins;
  /** level indicator */
  static int m_Level;
  /** window for displaying game stats*/
  static WINDOW * m_WinStats;

  /** y axis; height of the inner field */
  static int m_Height;
  /** x axis; width of the inner field */
  static int m_Width;
  /** window on the console */
  static WINDOW *m_Window;
  /** counts how many maps are there */
  static int m_Counter;

  /** direct pointer to the starting point */
  static std::shared_ptr<CDistinctPoint> m_Start;
  /** direct pointer to the ending point */
  static std::shared_ptr<CDistinctPoint> m_End;

  /**
   * @brief stores pointers to all enemy units in the game
   * Two dimensional array of lists of shared pointers to CEnemy
   * 1. param = width = x axis
   * 2. param = height = y axis
   * 3. param = depth = position in list
   */
  static std::unique_ptr<std::unique_ptr<std::list<std::shared_ptr<CEnemy>>[]>[]> m_EnemiesPosition;
  /** database of all enemies in the map */
  static std::list<std::shared_ptr<CEnemy>> m_Enemies;
  /** database of all towers in the map */
  static std::unordered_map<int, std::unordered_map<int, std::shared_ptr<CTower>>> m_Towers;
};

#endif