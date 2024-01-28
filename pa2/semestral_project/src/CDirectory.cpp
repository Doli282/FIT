#include "CDirectory.h"

#include "ncurses.h"
#include <filesystem>

bool CDirectory::listFiles(const std::string &path, const std::string &header, int originX, int originY) const
{
  try
  {
    int NOFiles = 0; // number of files in the folder
    int longestName = header.size();
    int currentLength = 0;

    // count files in the folder and find the longest name
    for (const auto &file : std::filesystem::directory_iterator(path))
    {
      if (file.is_regular_file())
      {
        NOFiles++;
        if ((currentLength = file.path().string().length()) > longestName)
        {
          longestName = currentLength;
        }
      }
    }
    // create window
    WINDOW *window = newwin(NOFiles + 2, longestName + 2, originX, originY);
    box(window, 0, 0);
    mvwprintw(window, 0, 1, header.c_str());
    // print filenames of files in the folder
    int y = 1; // y coordinate of the to-be-printed filename
    for (const auto &file : std::filesystem::directory_iterator(path))
    {
      mvwprintw(window, y++, 1, file.path().filename().c_str());
    }
    wrefresh(window);
    // finally deallocate memory
    delwin(window);
    return true;
  }
  catch (...)
  {
    return false;
  }
}