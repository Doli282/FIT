#include "CMap.h"
#include "CTowerWiki.h"
#include "CTower.h"
#include "CUnit.h"
#include "CParserMap.h"
#include "CCoordinates.h"
#include "CEnemy.h"
#include "CDistinctPoint.h"

//#include <cstdlib>
#include <fstream>
#include <string>
//#include <unordered_map>
#include <list>
#include <ncurses.h>
#include <memory>

#include <filesystem>

#include "CEnemyWiki.h"
#include "CEnemyBlindRight.h"
#include "CEnemyBlindLeft.h"
#include "CEnemyShortestAir.h"
#include "CEnemyShortest.h"
#include <iostream>
#include <sstream>

// static member variables
int CMap::m_Height;               // y axis; height of the inner field
int CMap::m_Width;                // x axis; width of the inner field
WINDOW *CMap::m_Window = nullptr; // window on the console
int CMap::m_Counter = 0;          // counts CMap instances
// std::unique_ptr<std::unique_ptr<std::shared_ptr<CUnit>[]>[]> CMap::m_Data; // stores pointers to all object in the game
std::shared_ptr<CDistinctPoint> CMap::m_Start;                                                    // direct pointer to the starting point
std::shared_ptr<CDistinctPoint> CMap::m_End;                                                      // direct pointer to the ending point
std::unordered_map<int, std::unordered_map<int, std::shared_ptr<CTower>>> CMap::m_Towers;         // stores pointers to all towers in the game
std::unique_ptr<std::unique_ptr<std::list<std::shared_ptr<CEnemy>>[]>[]> CMap::m_EnemiesPosition; // stores pointers to all enemy units in the game
std::list<std::shared_ptr<CEnemy>> CMap::m_Enemies;
int CMap::m_Lives;
int CMap::m_Score;
int CMap::m_Coins;
int CMap::m_Level;
WINDOW *CMap::m_WinStats = nullptr;
//------------------------------------------------------------

// convert the smallest of the ints to string
std::string CMap::minString(int value, int limit) const
{
  return (value > limit) ? std::to_string(limit) : std::to_string(value);
}
//------------------------------------------------------------

// display window with game statistics
bool CMap::displayStats() const
{
  if (m_WinStats == nullptr)
  {
    return false;
  }
  werase(m_WinStats);
  box(m_WinStats, 0, 0);
  mvwprintw(m_WinStats, 0, 1, "Game Stats");
  int limit = 999999999;
  mvwprintw(m_WinStats, 1, 1, ("Lives: " + minString(m_Lives, limit)).c_str());
  mvwprintw(m_WinStats, 2, 1, ("Score: " + minString(m_Score, limit)).c_str());
  mvwprintw(m_WinStats, 3, 1, ("Coins: " + minString(m_Coins, limit)).c_str());
  mvwprintw(m_WinStats, 4, 1, ("Level: " + minString(m_Level, limit)).c_str());
  wrefresh(m_WinStats);
  return true;
}
//------------------------------------------------------------

// refresh how the map looks like
bool CMap::display() const
{
  if (m_Window == nullptr)
  {
    return false;
  }
  werase(m_Window);
  box(m_Window, 0, 0);
  // print game stats
  displayStats();

  // print all towers
  for (auto &column : m_Towers)
  {
    for (auto &tower : column.second)
    {
      CCoordinates position = tower.second->getPosition();
      mvwaddch(m_Window, position.m_Y + 1, position.m_X + 1, tower.second->getSymbol());
    }
  }
  // print start and end
  mvwaddch(m_Window, m_Start->getPosition().m_Y + 1, m_Start->getPosition().m_X + 1, m_Start->getSymbol());
  mvwaddch(m_Window, m_End->getPosition().m_Y + 1, m_End->getPosition().m_X + 1, m_End->getSymbol());
  // print all enemies
  for (auto &enemy : m_Enemies)
  {
    mvwaddch(m_Window, enemy->getPosition().m_Y + 1, enemy->getPosition().m_X + 1, enemy->getSymbol());
  }
  wrefresh(m_Window);
  return true;
}
//------------------------------------------------------------

// erase tower from the list
bool CMap::dismantleTower(int x, int y, bool reimburse)
{
  try
  {
    if (reimburse)
    { // deleted intenionally -> get part of cost as reimbursement
      m_Coins += m_Towers.at(x).at(y)->getCost() * 0.6;
    }
    m_Towers.at(x).erase(y);
    if (reimburse)
    { // update state of the map
      display();
      displayStats();
    }
  }
  catch (...)
  {
    return false;
  }
  return true;
}
//------------------------------------------------------------

// place a tower
bool CMap::buildTower(char symbol, int winX, int winY)
{
  int x = winX - 1; // x according to the database
  int y = winY - 1; // y according to the database
  // check if field is empty
  CCoordinates position = CCoordinates(x, y);
  if (!isFieldEmpty(position) ||
      position == m_Start->getPosition() || position == m_End->getPosition())
  {
    throw "Field Occupied";
    return false;
  }
  // get tower type from wiki
  CTowerWiki wiki = CTowerWiki();
  std::shared_ptr<CTower> wikiTower = wiki.getCTower(symbol);
  if (wikiTower == nullptr)
  {
    throw "Not a Tower";
    return false;
  }
  // check funds
  if (m_Coins < wikiTower->getCost())
  {
    throw "Not enough funds";
    return false;
  }
  // create new instance and put it on the map
  std::shared_ptr<CTower> newTower = wikiTower->clone();
  newTower->setPosition(CCoordinates(x, y));
  m_Towers.at(x).insert({y, newTower});
  CEnemyWiki EWiki = CEnemyWiki();

  // check if it is not blocking all possible paths for enemies
  if (!(EWiki.resetPaths() && EWiki.preparePaths(false)))
  {
    dismantleTower(x, y);
    throw "Path must exist";
    return false;
  }
  m_Coins -= wikiTower->getCost(); // pay the price
  displayStats();
  mvwaddch(m_Window, winY, winX, symbol); // display it
  return true;
}
//------------------------------------------------------------

// building regime
bool CMap::buildingRegime()
{
  if (m_Window == nullptr)
  {
    return false;
  }
  keypad(m_Window, true);
  int keyPressed;
  int x = m_Width / 2;
  int y = m_Height / 2;
  wmove(m_Window, y, x);

  while ((keyPressed = wgetch(m_Window)) != 27)
  {
    switch (keyPressed)
    {
    case KEY_LEFT:
      x -= (x > 1 ? 1 : 0);
      break;
    case KEY_RIGHT:
      x += (x < m_Width ? 1 : 0);
      break;
    case KEY_UP:
      y -= (y > 1 ? 1 : 0);
      break;
    case KEY_DOWN:
      y += (y < m_Height ? 1 : 0);
      break;
    case KEY_DC: // Del key -> dismantle tower and get a part of the cost back
      dismantleTower(x - 1, y - 1, true);
      break;
    default: // any other key
      try
      {
        buildTower(keyPressed, x, y);
        // build successful
      }
      catch (char const *error)
      { // unable to build - print error
        displayStats();
        mvwprintw(m_WinStats, getmaxy(m_WinStats) - 1, 0, error);
        wrefresh(m_WinStats);
      }
      break;
    }
    wmove(m_Window, y, x);
  }
  return true;
}
//------------------------------------------------------------

// one turn in the game
bool CMap::turn()
{
  if (m_Window == nullptr)
  {
    return false;
  }
  CEnemyWiki wiki = CEnemyWiki();
  mvwprintw(m_WinStats, getmaxy(m_WinStats) - 1, 1, "Calculating");
  if (!wiki.preparePaths(true))
  {
    return false;
  }
  wrefresh(m_WinStats);

  int count = 6 + m_Level * 2; // number of spawns
  int NoEnemies = 0; // number of enemies currently on the map
  int enemiesInWave = 2 + m_Level / 3;
  curs_set(0); // invisible cursor
  while (NoEnemies > 0 || count > 0)
  { // repeat until no enemies reamin on the map

    if (count > 0)
    { // spawninf new units
      for (int w = 0; w < enemiesInWave; w++)
      {
        std::shared_ptr<CEnemy> newEnemy = wiki.createEnemy();
        if (newEnemy == nullptr)
        {
          return false;
        }
        newEnemy->setPosition(m_Start->getPosition());
        m_Enemies.push_back(newEnemy);
        NoEnemies++;
      }
      count--;
    }

    display();
    // wait n*tenths of sec
    halfdelay(5);
    getch();

    // ENEMY MOVEMENT
    // clear enemies positions
    resetEnemies();
    std::list<std::shared_ptr<CEnemy>>::iterator it = m_Enemies.begin();
    while (it != m_Enemies.end())
    { // for every enemy determine if its still alive, move it and place it on the map
      if ((*it)->isAlive())
      {
        (*it)->movement();
        m_EnemiesPosition[(*it)->getPosition().m_X][(*it)->getPosition().m_Y].push_back((*it));
        it++;
      }
      else
      {
        if (!(*it)->reachedEnd())
        {
          m_Score += (*it)->getPoints();
          m_Coins += (*it)->getPoints();
        }
        else
        {
          m_Lives--;
        }
        it = m_Enemies.erase(it);
        NoEnemies--;
      }
    }

    // TOWER FIRE
    for (auto &column : m_Towers)
    {
      for (auto &tower : column.second)
      {
        tower.second->fire(*this);
      }
    }
  }

  if(m_Lives > 0)
  {
    m_Level++;
  }
  display();
  // reset settings
  curs_set(1);
  cbreak();

  return true;
}
//------------------------------------------------------------

// check if field is occupied by a tower
bool CMap::isFieldEmpty(const CCoordinates &coordinates) const
{
  return isFieldEmpty(coordinates.m_X, coordinates.m_Y);
}
//------------------------------------------------------------

// check if a field is occupied by a tower
bool CMap::isFieldEmpty(int x, int y) const
{
  // check if it is not outside of the map
  if (x < 0 || y < 0 || x >= m_Width || y >= m_Height)
  {
    return false;
  }
  // checl of there is not a tower on this field
  if (m_Towers.at(x).find(y) != m_Towers.at(x).end())
  {
    return false;
  }
  return true;
}
//------------------------------------------------------------

// return pointer to list of enemies on a given field
std::shared_ptr<std::list<std::shared_ptr<CEnemy>>> CMap::getEnemiesAtField(int x, int y) const
{
  if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
  {
    return nullptr;
  }
  return std::make_shared<std::list<std::shared_ptr<CEnemy>>>(m_EnemiesPosition[x][y]);
}
//------------------------------------------------------------

// prepare map layout
bool CMap::initializeMap(const std::string &path, const std::string &fileName, int originX, int originY)
{
  if (!loadData(path + "/" + fileName))
  {
    return false;
  }
  // prepare new WINODW
  if (m_Window != nullptr)
  {
    delwin(m_Window);
  }
  m_Window = newwin(m_Height + 2, m_Width + 2, originY, originX);
  // draw a box around
  box(m_Window, 0, 0);
  wrefresh(m_Window);
  // initialize window for stats
  m_WinStats = newwin(6, 18, originY, originX + m_Width + 2);
  box(m_WinStats, 0, 0);
  wrefresh(m_WinStats);
  return true;
}
//------------------------------------------------------------

// reset database for enemies
void CMap::resetEnemies()
{
  m_EnemiesPosition = std::make_unique<std::unique_ptr<std::list<std::shared_ptr<CEnemy>>[]>[]>(m_Width);
  for (int x = 0; x < m_Width; x++)
  {
    // prepare columns of nullpointers and move them to m_Data array
    m_EnemiesPosition[x] = std::make_unique<std::list<std::shared_ptr<CEnemy>>[]>(m_Height);
  }
  return;
}
//------------------------------------------------------------

// prepare database for towers
void CMap::resetTowers()
{
  std::unordered_map<int, std::shared_ptr<CTower>> column;
  column.reserve(m_Height);
  m_Towers.reserve(m_Width);
  for (int i = 0; i < m_Width; i++)
  {
    m_Towers.insert({i, column});
  }
}
//------------------------------------------------------------

// load Data from CMap data file
bool CMap::loadData(const std::string &path)
{
  std::ifstream fstream(path);
  if (!fstream.is_open())
  { // unable to open stream from/to the data file
    throw "unable to open";
    return false;
  }

  CParserMap parser = CParserMap(); // contains parsing functions
  std::string input;                // first number -> later x axis

  // Score
  fstream >> input;
  if (!parser.parseNumber(m_Score, input))
  {
    throw "unable to parse score";
    return false;
  }

  // Coins
  fstream >> input;
  if (!parser.parseNumber(m_Coins, input))
  {
    throw "unable to parse coins";
    return false;
  }

  // Lives
  fstream >> input;
  if (!parser.parseNumber(m_Lives, input))
  {
    throw "unable to parse lives";
    return false;
  }

  // Level
  fstream >> input;
  if (!parser.parseNumber(m_Level, input))
  {
    throw "unable to parse level";
    return false;
  }

  // parse Width
  fstream >> input;
  if (!parser.parseNumber(m_Width, input))
  {
    throw "unable to parse width";
    return false;
  }

  // parse Height
  fstream >> input;
  if (!parser.parseNumber(m_Height, input))
  {
    throw "unable to parse height";
    return false;
  }

  // create empty map
  // reset();

  CCoordinates coordinates;
  std::string input2; // second number -> y axis
  // parse Start
  fstream >> input;
  fstream >> input2;
  if (!parser.parseDistinctPoint(coordinates, input, input2, m_Width, m_Height))
  {
    throw "unable to parse start";
    return false;
  }
  m_Start = std::make_shared<CDistinctPoint>(coordinates, '<');
  m_Start->specifySymbol(m_Width, true);
  // m_Data[coordinates.m_X][coordinates.m_Y] = m_Start;

  // parse End
  fstream >> input;
  fstream >> input2;
  if (!parser.parseDistinctPoint(coordinates, input, input2, m_Width, m_Height))
  {
    throw "unable to parse end";
    return false;
  }
  m_End = std::make_shared<CDistinctPoint>(coordinates, '<');
  m_End->specifySymbol(m_Width, false);
  // m_Data[coordinates.m_X][coordinates.m_Y] = m_End;

  resetEnemies();
  resetTowers();
  // parse CUnits
  std::string towerSymbol;
  while (fstream >> towerSymbol)
  {
    fstream >> input;
    fstream >> input2;
    // check the symbol of the CTower
    std::shared_ptr<CTower> newTower;
    if (!parser.parseCTower(towerSymbol, newTower, coordinates, input, input2, m_Width, m_Height))
    {
      throw "unable to parse tower";
      return false;
    }
    // if everything was OK, finally construct new CUnit
    newTower->setPosition(coordinates);
    m_Towers.at(coordinates.m_X).insert({coordinates.m_Y, newTower});
    // m_Data[coordinates.m_X][coordinates.m_Y] = newTower;
  }
  return true;
}
//------------------------------------------------------------

// save game to CMap data file
int CMap::saveData(const std::string &path, const std::string &fileName, bool overwrite) const
{
  if (path.empty() || fileName.empty())
  {
    return 2;
  }
  // check if the file already exists
  if (!overwrite && std::filesystem::exists(path + "/" + fileName))
  {
    return 1;
  }

  std::ofstream file(path + "/" + fileName);
  if (!file.is_open())
  { // problem with stream initialization
    return 2;
  }

  // write the data down
  file << printData() << std::flush;

  if (!file.good())
  { // some error occured
    return 2;
  }
  // std::cout << "save completed" << std::endl;
  return 0;
}
//------------------------------------------------------------

// print all data from CMap
std::string CMap::printData() const
{
  std::stringstream ss;
  // print internal game and CMap data data
  ss << m_Score << "\n"
     << m_Coins << "\n"
     << m_Lives << "\n"
     << m_Level << "\n"
     << std::endl;
  ss << m_Width << "\n"
     << m_Height << "\n"
     << std::endl;
  ss << m_Start->print() << "\n";
  ss << m_End->print() << "\n"
     << std::endl;

  // print towers
  for (auto &column : m_Towers)
  {
    for (auto &tower : column.second)
    {
      ss << tower.second->print() << std::endl;
    }
  }
  return ss.str();
}
//------------------------------------------------------------