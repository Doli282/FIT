#ifndef CENEMYWIKI_H
#define CENEMYWIKI_H

#include "CEnemy.h"

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <ncurses.h>

/**
 * @brief Wiki with list of all different kinds of enemy units
 *
 */
class CEnemyWiki
{
public:
  /**
   * @brief Construct a new CEnemyWiki object
   *
   */
  CEnemyWiki() { m_Counter++; }

  /**
   * @brief Destroy the CEnemyWiki object
   *
   */
  ~CEnemyWiki()
  {
    m_Counter--;
    if (m_Counter <= 0 && m_Window != nullptr)
    {
      delwin(m_Window);
    }
  }

  /**
   * @brief Initialize wiki, load data from files
   *
   * @param path [in] path to directory with data files
   * @param originX [in] x coordinate of the origin
   * @param originY [in] y coordnate of the origin
   * @return true - success
   * @return false - problem occured - no CEnemy was found
   */
  bool initialize(const std::string &path, int originX = 0, int originY = 0);

  /**
   * @brief Display wiki in a table
   *
   * @return true - success
   * @return false - wiki has not been initialized
   */
  bool display() const;

  /**
   * @brief Check if a symbol is used by an enemy unit
   *
   * @param symbol [in] examined symbol
   * @return true - is used
   * @return false - unused
   */
  bool isInWiki(char symbol) const;

  /**
   * @brief Get the pointer to enemy with given symbol
   *
   * @param symbol [in] searched-for symbol
   * @return std::shared_ptr<CEnemy> to found enemy or nullptr if nothing was found
   */
  std::shared_ptr<CEnemy> getEnemy(char symbol) const;

  /**
   * @brief Create new instance of random enemy class
   *
   * @return std::shared_ptr<CEnemy>  (nullptr if there are none)
   */
  std::shared_ptr<CEnemy> createEnemy() const;

  /**
   * @brief Forget paths of all enemies
   *
   * @return true
   * @return false
   */
  bool resetPaths() const;

  /**
   * @brief Prepare new paths for all enemies
   *
   * @param all true = all paths; false = smart paths
   * @return true - paths found
   * @return false - END is unreachable
   */
  bool preparePaths(bool all) const;

  /**
   * @brief Get the Width object
   *
   * @return int
   */
  int getWidth() { return m_Width; }

protected:
  /** set of all enemy varieties */
  static std::map<char, std::shared_ptr<CEnemy>> m_CEnemyList;
  /** vector with all enemy types */
  static std::vector<std::shared_ptr<CEnemy>> m_CEnemyVector;
  /** number of different enemy types */
  static int m_Varieties;
  /** working window */
  static WINDOW *m_Window;
  /** header of table */
  static std::string m_Header;
  /** width of the whole window */
  static int m_Width;
  /** how many instances of CEnemyWiki are there */
  static int m_Counter;
};

#endif