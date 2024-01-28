#include "CMap.h"
#include "CTowerWiki.h"
#include "CEnemyWiki.h"
#include "CInputReader.h"
#include "CDirectory.h"
#include "CStartingMenu.h"

#include <ncurses.h>
#include <string>
#include <iostream>

// initialize paths
#define pathMap "./examples/CMap_game1.txt"
#define pathSavedGames "./examples/savedGames"
#define pathTower "./examples/towers"
#define pathEnemy "./examples/enemies"
#define listHeader "Saved Games:"
#define pathToHelp "./examples/Help.txt"

#define promptSave "Save game - Enter name of the file:"
#define promptLoad "Load game - Enter name of the file:"

// important classes declaration
CDirectory dir = CDirectory();
CMap map = CMap();
CTowerWiki wiki = CTowerWiki();
CEnemyWiki enemyWiki = CEnemyWiki();
CInputReader inputReader = CInputReader(20);
CStartingMenu mainMenu = CStartingMenu();

/**
 * @brief Redraw elements on the screen
 */
void redraw()
{
  erase();
  refresh();
  inputReader.drawBox();
  wiki.display();
  enemyWiki.display();
  map.display();
}

/**
 * @brief Initialize CTowerWiki a CEnemyWiki
 *
 * @return true - success
 * @return false - corrupted files
 */
bool initializeWiki()
{
  if (!wiki.initialize(pathTower, 0, inputReader.getHeight()))
  {
    inputReader.printMessage("ERR: no valid CTower data files found");
    inputReader.printMessage("Exiting application ... ", 2, 5, " (press any key to exit)");
    return false;
  }
  if (!enemyWiki.initialize(pathEnemy, 0, inputReader.getHeight() + wiki.getHeight()))
  {
    inputReader.printMessage("ERR: no valid CEnemy data files found");
    inputReader.printMessage("Exiting application ... ", 2, 5, " (press any key to exit)");
    return false;
  }
  return true;
}

/**
 * @brief Save Game procedure with check for overwrite
 *
 */
void saveGame()
{
  int check;
  std::string buffer;
gSave:
  buffer.clear();
  // list files in saving directory
  dir.listFiles(pathSavedGames, listHeader, 5, 10);
  // read user input
  if (!inputReader.userInput(buffer, promptSave))
  { // input closed prematurely - return back
    return;
  }
  if (buffer.empty())
  { // do not accept empty buffer as answer - repeat
    goto gSave;
  }
  // save game according to user input
  check = map.saveData(pathSavedGames, buffer);
  switch (check)
  {
  case 0: // everything OK
    break;
  case 1: // file already exist -> check for overwrite decision
    inputReader.printMessage("File Exists - Overwirte? - [Y]es/ [N]o", 2);
    while ((check = getch())) // wait for confirmation
    {
      switch (check)
      {
      case 'y':
      case 'Y':
        // want to overwrite
        if (map.saveData(pathSavedGames, buffer, true) == 2)
        { // there was some problem -> try the whole saving procedure once more
          inputReader.drawBox();
          inputReader.printMessage("Problem occured - try again (press any key)", 2);
          getchar();
          goto gSave;
        }
        // successfully saved
        goto gSaved;
      case 'n':
      case 'N':
        // decline to overwrite -> try again
        goto gSave;
      default: // other pressed keys are ignored
        break;
      }
    }
    break;
  default: // case 2
    inputReader.printMessage("Problem occured - try again (press any key)", 2);
    getchar();
    goto gSave;
    break;
  }
gSaved:
  redraw();
  inputReader.printMessage("Game saved  (press to return)", 2);
  getchar();
  return;
}

/**
 * @brief Print Game OVer message
 *
 */
void gameOver()
{
  inputReader.printMessage("GAME OVER", 1);
  inputReader.printMessage("Press any key to return", 2);
  getch();
}

int main()
{
  // ncurses initialization
  initscr();
  noecho();
  raw();
  keypad(stdscr, TRUE);
  cbreak();
  refresh();

  // variables declaration
  int keyPressed;

  // initialize InputReader
  std::string buffer;

  inputReader.resize(getmaxx(stdscr), 3);

  // initialize CTowerWiki & CEnemyWiki -> load data from data files
  if (!initializeWiki())
  { // terminal error
    getchar();
    goto gEnd;
  }

  while (true)
  {
  gMenu:
    // show mainmenu (show help if needed)
    if (mainMenu.mainMenu(pathToHelp) == 2)
    { // option 'exit' has been chosen
      goto gEnd;
    }

    // Load game according to user input
    dir.listFiles(pathSavedGames, listHeader, inputReader.getHeight());
    while (true)
    {
      if (!inputReader.userInput(buffer, promptLoad))
      { // end of input -> go back
        goto gMenu;
      }
      // initialize map - lay CUnits out and display the map
      try
      {
        map.initializeMap(pathSavedGames, buffer, wiki.getWidth(), inputReader.getHeight());
        break;
      }
      catch (char const *a)
      {
        inputReader.printMessage(a, 2, 1, " - press key");
        getch();
        continue;
      }
    }

    // redraw the screen
    redraw();
    while (map.stillAlive())
    {
      keyPressed = getch();
      switch (keyPressed)
      {
      case 'h':
      case 'H':
        inputReader.printMessage("n = next turn, b = build, s = save, q = quit");
        inputReader.printMessage("(press key to return)", 2);
        getch();
        break;
      case 'n':
      case 'N':
        // Next turn in the game
        if (!map.turn())
        {
          goto gMenu;
        }
        break;
      case 'q':
      case 'Q':
        // Quit the game
        goto gMenu;
      case 's':
      case 'S':
        // Save the game
        saveGame();
        break;
      case 'b':
      case 'B':
        // Building regime
        inputReader.printMessage("Building Regime (ESC to return)", 1);
        map.buildingRegime();
        break;
      default: // ignore other keys
        break;
      }
      // redraw the screen
      redraw();
    }

    // GAME OVER
    gameOver();
  }
// before exiting the application
gEnd:
  endwin();

  return 0;
}