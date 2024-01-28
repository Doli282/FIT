#include "CEnemyWiki.h"
#include "CEnemy.h"
#include "CEnemyBlindLeft.h"
#include "CEnemyBlindRight.h"
#include "CEnemyShortest.h"
#include "CEnemyShortestAir.h"
#include "CParser.h"

#include <string>
#include <map>
#include <memory>
#include <ncurses.h>
#include <filesystem>
#include <fstream>

std::map<char, std::shared_ptr<CEnemy>> CEnemyWiki::m_CEnemyList;
std::vector<std::shared_ptr<CEnemy>> CEnemyWiki::m_CEnemyVector;
int CEnemyWiki::m_Varieties = 0;
WINDOW *CEnemyWiki::m_Window = nullptr;
std::string CEnemyWiki::m_Header = "sym-HP-path-type-pts";
int CEnemyWiki::m_Width = m_Header.length() + 2;
int CEnemyWiki::m_Counter = 0;

bool CEnemyWiki::initialize(const std::string &path, int originX, int originY)
{
  m_Varieties = 0;
  m_CEnemyList.clear();
  m_CEnemyVector.clear();
  CParser parser = CParser();
  // goes through every file in directory
  for (const auto &file : std::filesystem::directory_iterator(path))
  {
    std::shared_ptr<CEnemy> newEnemy;
    // file has desired name -> "CEnemy...."
    if ((parser.nameMatch(file.path().filename(), "CEnemyBlindRight_")))
    {
      newEnemy = std::make_shared<CEnemyBlindRight>('0');
    }
    else if ((parser.nameMatch(file.path().filename(), "CEnemyBlindLeft_")))
    {
      newEnemy = std::make_shared<CEnemyBlindLeft>('0');
    }
    else if ((parser.nameMatch(file.path().filename(), "CEnemyShortest_")))
    {
      newEnemy = std::make_shared<CEnemyShortest>('0');
    }
    else if ((parser.nameMatch(file.path().filename(), "CEnemyShortestAir_")))
    {
      newEnemy = std::make_shared<CEnemyShortestAir>('0');
    }
    else
    {
      continue;
    }

    // file is regular file
    if (!file.is_regular_file())
    {
      continue;
    }
    std::fstream fs(file.path());

    // stream to/from file has been successfully opened
    if (!fs.is_open())
    {
      continue;
    }
    std::string output;
    char m_Symbol;

    // extract symbol
    fs >> output;
    if (output.length() != 1)
    {
      continue;
    }
    m_Symbol = output.at(0);

    // m_CEnemyList already constains this symbol
    if (m_CEnemyList.find(m_Symbol) != m_CEnemyList.end())
    {
      // std::cout << "<" << m_Symbol << "> is already used. Please check the data file <" << file << ">" << std::endl;
      continue;
    }

    // create new CEnemy to be placed in the m_CEnemyList
    const int arg = 4;   // number of arguments
    int parameters[arg]; // 4 as three parameters in CEnemy
    parameters[0] = static_cast<int>(m_Symbol);
    int iterator = 1;
    bool fineFile = true; // file has the correct format
    while (fineFile && (fs >> output))
    {
      if (iterator >= arg)
      {
        fineFile = false;
        // std::cout << "too many arguments" << std::endl;
        break;
      }
      if (!parser.parseNumber(parameters[iterator], output))
      {
        fineFile = false;
        // std::cout << "<" << output << "> is not a number" << std::endl;
        break;
      }
      iterator++;
    }
    // file was successfully parsed
    if (fineFile)
    {
      if (!newEnemy->setValues(parameters))
      {
        // std::cout << "arguments out of bound" << std::endl;
        continue;
      }
      m_CEnemyVector.push_back(newEnemy);
      m_CEnemyList.emplace(m_Symbol, newEnemy);
      m_Varieties++;
    }
  }
  // no CEnemy file has been parsed
  if (m_Varieties < 1)
  {
    return false;
  }

  // allocate window
  if (m_Window != nullptr)
  {
    delwin(m_Window);
  }
  m_Window = newwin(m_Varieties + 2, m_Width, originY, originX);

  return true;
}
//------------------------------------------------------------

// print CEnemy wiki
bool CEnemyWiki::display() const
{
  // check if wiki was initialized first
  if (m_Window == nullptr)
  {
    return false;
  }
  werase(m_Window);
  // draw box arnoud the table
  box(m_Window, 0, 0);
  // print header of the table
  mvwprintw(m_Window, 0, 1, m_Header.c_str());
  int y = 1; // row number
  // print entries (CEnemy stats) in the wiki
  //std::cout <<"vypis" << std::endl;
  for (const auto &enemy : m_CEnemyList)
  {
    //std::cout << "bab" << std::endl;
    mvwprintw(m_Window, y, 1, enemy.second->printValues().c_str());
    //std::cout << "asa" << std::endl;
    y++;
  }
  // refresh the image
  wrefresh(m_Window);
  return true;
}
//------------------------------------------------------------

// determine if a symbol is already used
bool CEnemyWiki::isInWiki(char symbol) const
{
  return (m_CEnemyList.find(symbol) != m_CEnemyList.end());
}
//------------------------------------------------------------

// return CEnemy with given symbol
std::shared_ptr<CEnemy> CEnemyWiki::getEnemy(char symbol) const
{
  if (isInWiki(symbol))
  {
    return m_CEnemyList.at(symbol);
  }
  return nullptr;
}
//------------------------------------------------------------

// return pointer to new instance of CEnemy
std::shared_ptr<CEnemy> CEnemyWiki::createEnemy() const
{
  if (m_Varieties < 1)
  {
    return nullptr;
  }
  return m_CEnemyVector.at(std::rand() % m_Varieties)->clone();
}

//------------------------------------------------------------

// make enemies forget their path
bool CEnemyWiki::resetPaths() const
{
  for (auto &enemy : m_CEnemyList)
  {
    enemy.second->resetPath();
  }
  return true;
}
//------------------------------------------------------------

// let enemies prepare their paths
bool CEnemyWiki::preparePaths(bool all) const
{
  CMap map = CMap();
  for (auto &enemy : m_CEnemyList)
  { // let firstly searhc for the path the smart enemies
    if (enemy.second->isSmart())
    {
      if (!enemy.second->calculatePath(map))
      { // smart enemy could not find its path -> there must be none
        return false;
      }
    }
  }
  if (all)
  {
    for (auto &enemy : m_CEnemyList)
    { // then let the others search for their path
      if (!enemy.second->calculatePath(map))
      { // to be sure that everone has prepared their paths
        return false;
      }
    }
  }
  return true;
}
//------------------------------------------------------------