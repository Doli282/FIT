#include "CTowerWiki.h"
#include "CTower.h"
#include "CTowerSplash.h"
#include "CParser.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <ncurses.h>
#include <memory>

std::map<char, std::shared_ptr<CTower>> CTowerWiki::m_CTowerList;
WINDOW *CTowerWiki::m_Window = nullptr;
int CTowerWiki::m_Count = 0;
std::string CTowerWiki::m_Header = "sym-type-dmg-cld-rng-cost";
int CTowerWiki::m_Width = m_Header.length() + 2;
int CTowerWiki::m_Counter = 0;

// load dat from files
bool CTowerWiki::initialize(const std::string &path, int originX, int originY)
{
  m_Count = 0;
  m_CTowerList.clear();
  CParser parser = CParser();
  // goes through every file in directory
  for (const auto &file : std::filesystem::directory_iterator(path))
  {
    // file has desired name -> "CTower...."
    std::shared_ptr<CTower> newTower;
    if ((parser.nameMatch(file.path().filename(), "CTower_")))
    {
      newTower = std::make_shared<CTower>('0');
    }
    else if ((parser.nameMatch(file.path().filename(), "CTowerSplash_")))
    {
      newTower = std::make_shared<CTowerSplash>('0');
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

    // m_CTowerList already constains this symbol
    if (m_CTowerList.find(m_Symbol) != m_CTowerList.end())
    {
      // std::cout << "<" << m_Symbol << "> is already used. Please check the data file <" << file << ">" << std::endl;
      continue;
    }

    // create new CTower to be placed in the m_CTowerList
    const int arg = 6;   // number of arguments
    int parameters[arg]; // parameters in CTower
    parameters[0] = static_cast<int>(m_Symbol);
    int iterator = 1;     // moved becuase [0] is the symbol
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
      if (!newTower->setValues(parameters))
      {
        // std::cout << "invalid type" << std::endl;
        continue;
      }
      m_CTowerList.emplace(m_Symbol, newTower);
      m_Count++;
    }
  }
  // no CTower file has been parsed
  if (m_Count < 1)
  {
    return false;
  }

  // allocate window
  if (m_Window != nullptr)
  {
    delwin(m_Window);
  }
  m_Window = newwin(m_Count + 2, m_Width, originY, originX);

  return true;
}

// print CTower wiki
bool CTowerWiki::display() const
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
  // print entries (CTower stats) in the wiki
  for (const auto &tower : m_CTowerList)
  {
    mvwprintw(m_Window, y, 1, tower.second->printValues().c_str());
    y++;
  }
  // refresh the image
  wrefresh(m_Window);
  return true;
}

// check if symbol is in the list
bool CTowerWiki::isInWiki(char symbol) const
{
  if (m_CTowerList.find(symbol) == m_CTowerList.end())
  {
    return false;
  }
  return true;
}

std::shared_ptr<CTower> CTowerWiki::getCTower(char symbol)
{
  if (!isInWiki(symbol))
  {
    return nullptr;
  }
  return m_CTowerList.at(symbol);
}