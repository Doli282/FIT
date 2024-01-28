#include "CStartingMenu.h"

#include <ncurses.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

// Possible implementation of the starting menu
int CStartingMenu::mainMenu(const std::string &path) const
{
  // source of inspiration: https://www.youtube.com/watch?v=3YiPdibiQHA&list=PL2U2TQ__OrQ8jTf0_noNKtHMuYlyxQl4v&index=9
  erase();
  refresh();

  int yMax, xMax;
  getmaxyx(stdscr, yMax, xMax);

  // creates a window for the menu
  int menuHeight, menuWidth, menuY, menuX;
  menuHeight = 10;
  menuWidth = 15;
  menuX = (xMax - menuWidth) / 2;
  menuY = (yMax - menuHeight) / 2;
  WINDOW *menuwin = newwin(menuHeight, menuWidth, menuY, menuX);
  box(menuwin, 0, 0);
  wrefresh(menuwin);

  // makes it so we can use arrow keys, and make cursor invisible
  keypad(menuwin, true);
  curs_set(0);

  // define options in the menu
  const int optionsNO = 3;
  std::string options[optionsNO] = {"Load game", "Help", "Exit"};
  int keyPressed;    // key pressed by the user
  int highlight = 0; // highlighted option as if currently choosed

  // loop waiting for user to choose option
  while (1)
  {
    // highlight selected option
    for (int i = 0; i < optionsNO; i++)
    {
      if (i == highlight)
      {
        wattron(menuwin, A_REVERSE);
      }
      mvwprintw(menuwin, i + 1, 1, options[i].c_str());
      wattroff(menuwin, A_REVERSE);
    }
    wrefresh(menuwin);
    // determine what key was pressed and move the cursor accordingly, 'Enter' ends this loop
    keyPressed = wgetch(menuwin);
    switch (keyPressed)
    {
    case KEY_UP:
      highlight--;
      if (highlight == -1)
      { // to stay in the boundaries
        highlight = 0;
      }
      break;
    case KEY_DOWN:
      highlight++;
      if (highlight == optionsNO)
      { // to stay in the boundaries
        highlight = optionsNO - 1;
      }
      break;
    default:
      break;
    }
    // Pressed key was 'Enter' (10) -> end the loop
    if (keyPressed == 10)
    {
      if (highlight == 1)
      {
        showHelp(path);
        box(menuwin, 0, 0);
        continue;
      }
      break;
    }
  }
  delwin(menuwin);
  curs_set(1);
  erase();
  refresh();
  return highlight;
}

bool CStartingMenu::showHelp(const std::string &path) const
{
  erase();
  refresh();
  std::fstream file(path);
  if (!file.is_open())
  {
    mvprintw(0, 0, "ERR: unable to open HELP file");
    return false;
  }
  std::vector<std::string> text;
  std::string line;
  int lines = 0;
  // save the file to the vector by lines
  while (std::getline(file, line))
  {
    text.push_back(line);
    lines++;
  }
  int iterator = 0; // iterator in vector -> the first printed line
  int max = getmaxy(stdscr) - 1; // maximal number of printed lines
  int maxit = (lines - max) <= 0 ? 0 : (lines - max); // maximal iterator -> last first printed line
  int ch = 0;
  do
  {
    erase();
    refresh();
    // print text
    for(int i = 0; i < max && i < lines; i++)
    {
      mvprintw(i, 0, text.at(iterator+i).c_str());
    }
    mvprintw(max, 0, "[ESC] return");
    // if arrow UP/DOWN was presed, scroll text, if not scrolled to the maximum already
    switch(ch)
    {
      case KEY_DOWN:
        iterator++;
        if(iterator > maxit)
        {
          iterator = maxit;
        }
        break;
      case KEY_UP:
        iterator--;
        if(iterator < 0)
        {
          iterator = 0;
        }
        break;
      default:
      break;
    }
  
  } while((ch = getch()) != 27);// 27 = ESC -> end
  erase();
  refresh();
  return true;
}