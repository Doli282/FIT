#include "CInputReader.h"

#include <ncurses.h>
#include <string>

bool CInputReader::drawBox() const
{
  if (m_Window != nullptr)
  {
    werase(m_Window);
    box(m_Window, 0, 0);
    wrefresh(m_Window);
    return true;
  }
  return false;
}

bool CInputReader::resize(int width, int height, int originX, int originY)
{
  if(width > getmaxx(stdscr) || width < 3 || height < 3 || height > getmaxy(stdscr))
  {
    return false;
  }
  if(m_Window != nullptr)
  {
    delwin(m_Window);
  }
  m_Window = newwin(height, width, originY, originX);
  m_Height = height;
  m_Width = width;
  return true;
}

// delete character form string
void CInputReader::deleteFromBuffer(std::string &buffer, int iterator, int currLen)
{
  for (; iterator < currLen; iterator++)
  { // shift characters
    buffer[iterator] = buffer[iterator + 1];
  }
  // shift the end of string
  // buffer[currLen] = '\0';
  buffer.pop_back();
  return;
}

// insert char in the string
void CInputReader::insertInBuffer(std::string &buffer, int insertPoint, int currLen, int input)
{
  // push the ending zero one position back
  // buffer[currLen+1] = '\0';
  buffer.push_back(buffer[--currLen]);
  for (; currLen >= insertPoint; currLen--)
  { // shift all elements until you reach the insertion point
    buffer[currLen] = buffer[currLen - 1];
  }
  // insert new char
  buffer[insertPoint] = (char)input;
}


bool CInputReader::printMessage(const std::string &message, int originY, int originX, const std::string &tail) const
{
  if(m_Window == nullptr)
  {
    return false;
  }
  mvwprintw(m_Window, originY, originX, (message + tail).c_str());
  wrefresh(m_Window);
  return true;
}

// read user input in a string format
bool CInputReader::userInput(std::string &buffer, const std::string &prompt)
{
  return userInput(buffer, m_Width - 2, m_Window, prompt);
}

// read user input in a string format
bool CInputReader::userInput(std::string &buffer, int maxLen, WINDOW *window, const std::string &prompt, int initialX, int y)
{
  if (window == nullptr)
  {
    return false;
  }
  // buffer preparation
  buffer.clear();
  buffer.reserve(maxLen + 1);
  int x = initialX; // current x coordinate of the cursor
  int currLen = 0;  // current length of the text
  int keyPressed;
  std::string fill; // string used for clearing the input text field
  fill.assign(maxLen, ' ');

  // initial preparation of the input text field
  mvwprintw(window, y, initialX, fill.c_str());
  box(window, 0, 0);
  mvwprintw(m_Window, 0, 1, prompt.c_str());
  wmove(window, y, initialX);
  wrefresh(window);
  while ((keyPressed = getch()))
  {
    switch (keyPressed)
    {
    // left arrow -> move cursor to the left
    case KEY_LEFT:
      if (x > initialX)
      {
        x--;
      }
      break;
    // right arrow -> move cursor to the right
    case KEY_RIGHT:
      if (x <= currLen)
      {
        x++;
      }
      break;
    // BACKSPACE key -> remove character
    case KEY_BACKSPACE:
      if (x > initialX)
      {
        x--;
        deleteFromBuffer(buffer, x - initialX, --currLen);
        // clear following symbols and reprint
        mvwprintw(window, y, initialX, fill.c_str());
        mvwprintw(window, y, initialX, buffer.c_str());
      }
      break;
    // DEL key -> remove character
    case KEY_DC:
      if (currLen > 0 && x <= currLen)
      {
        deleteFromBuffer(buffer, x - 1, --currLen);
        // clear following symbols and reprint
        mvwprintw(window, y, initialX, fill.c_str());
        mvwprintw(window, y, initialX, buffer.c_str());
      }
      break;
    // end of reading input
    case 27:   // ESC key
    case 0x04: // ctr+D
      return false;
    case KEY_ENTER: // ENTER key
    case '\n':      // newline
      return true;
    // other characters are put into the buffer and printed
    default:
      if (currLen < maxLen)
      {
        if (std::isprint(keyPressed)) // only printable symbols are stored
        {
          insertInBuffer(buffer, x - initialX, currLen++, keyPressed);
          x++;
          mvwprintw(window, y, initialX, buffer.c_str());
        }
      }
      break;
    }
    // set cursors and refresh the window
    wmove(window, y, x);
    wrefresh(window);
  }
  return false;
}
