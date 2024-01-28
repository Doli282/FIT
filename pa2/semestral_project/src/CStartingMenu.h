#ifndef CSTARTINGMENU_H
#define CSTARTINGMENU_H

#include <string>

class CStartingMenu
{
public:
  /**
   * @brief Open MainMenu with choices
   *
   * @param path [in] path to Help file
   * @return int 0=LoadGame, 1=Help, 2=Exit
   */
  int mainMenu(const std::string &path) const;

protected:
  /**
   * @brief Print content of Help file to the screen
   *
   * @param path [in] path to Help file
   * @return true
   * @return false file with help could not be open
   */
  bool showHelp(const std::string &path) const;
};

#endif