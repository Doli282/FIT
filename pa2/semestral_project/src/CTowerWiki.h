#ifndef CTOWERWIKI_H
#define CTOWERWIKI_H

#include "CTower.h"

#include <map>
#include <ncurses.h>
#include <memory>

/**
 * @brief wiki with list of all kinds of possible towers
 *
 */
class CTowerWiki
{
public:
  /**
   * @brief Construct a new CTowerWiki object
   *
   */
  CTowerWiki()
  {
    m_Counter++;
  }

  /**
   * @brief Destroy the CTowerWiki object
   *
   */
  ~CTowerWiki()
  {
    m_Counter--;
    if (m_Counter <= 0 && m_Window != nullptr)
    {
      delwin(m_Window);
    }
  }

  /**
   * @brief Load all CTowers from data files in the directory
   *  -> CTowers data must be stored in data file with prefix 'CTower_'
   *
   * @param path [in] directory searched for data files
   * @return true - at least one CTower successfully parsed
   * @return false - no valid data file found
   */
  bool initialize(const std::string &path, int originX = 0, int originY = 0);

  /**
   * @brief Print CTower wiki list on the screen to prepare window
   *
   * @return true - printed successfully
   * @return false - something went wrong (not a possible outcome)
   */
  bool display() const;

  /**
   * @brief Check if CTower with given symbol exists
   *
   * @param symbol [in] symbol of desired CTower
   * @return true - CTower with given symbol exists
   * @return false - CTower with given symbol does not exist
   */
  bool isInWiki(char symbol) const;

  /**
   * @brief get CTower with corresponding symobl
   *
   * @param symbol [in] key to determine the CTower
   * @return std::shared_ptr<CTower> - pointer to the CTower, nullptr if nothing is found
   */
  std::shared_ptr<CTower> getCTower(char symbol);

  /**
   * @brief Get the m_Width object
   *
   * @return int
   */
  int getWidth() const { return m_Width; }

  /**
   * @brief Get the height of the window
   *
   * @return int
   */
  int getHeight() const { return m_Count + 2; }

protected:
  /** stores all existing CTower varieties */
  static std::map<char, std::shared_ptr<CTower>> m_CTowerList;
  /** assigned WIDNOW where the wiki will be shown */
  static WINDOW *m_Window;
  /** amount of different CTowers in the list */
  static int m_Count;
  /** header above the CTower wiki list */
  static std::string m_Header;
  /** length of the header + 2 for borders */
  static int m_Width;
  /** counts how many wikis are there */
  static int m_Counter;
};

#endif