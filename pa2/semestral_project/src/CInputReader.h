#ifndef CINPUTREADER_H
#define CINPUTREADER_H

#include <ncurses.h>
#include <string>

/**
 * @brief Class containing functions for parsing user input
 *
 */
class CInputReader
{
public:
  /**
   * @brief Construct a new CInputReader object
   *
   * @param width [in] width of the whole window (-2 for borders)
   * @param height [in] height of the whole window
   * @param originX [in] x coordinate of the originof the window
   * @param originY [in] y coordinate of the origin of the window
   */
  CInputReader(int width = 0, int height = 3, int originX = 0, int originY = 0) : m_Width(width), m_Height(height)
  {
    // prepare new window
    m_Window = newwin(m_Height, m_Width, originY, originX);
  }

  /**
   * @brief Destroy the CInputReader object
   *
   */
  ~CInputReader()
  {
    // delete window
    delwin(m_Window);
  }

  /**
   * @brief Read USER input in a string format
   *  !!! ncurses initialization NECESSARY -> noecho(); raw(); keypad(stdscr, TRUE); cbreak(); !!!
   * Supported keys are BACKSPACE, DEL, ESC, CTR+D, LEFT ARROW, RIGHT ARROW, ENTER
   * Stores only printable characters
   *
   * @param buffer [out] string where the input will be stored - will be overwriten
   * @param maxLen [in] maximal length of the buffer
   * @param window [in] window with the text
   * @param prompt [in] string printed above the input line - should be shorter then the m_Width of the line
   * @param initialX [in] initial position of the text on the x axis
   * @param y [in] y coordinate of the text
   * @return true - input confirmed -> ENTER key or newline
   * @return false - input closed prematurely -> CTR+D or ESC key
   */
  bool userInput(std::string &buffer, int maxLen, WINDOW *window, const std::string &prompt = "", int initialX = 1, int y = 1);

  /**
   * @brief Read USER input in a string format
   *  !!! ncurses initialization NECESSARY -> noecho(); raw(); keypad(stdscr, TRUE); cbreak(); !!!
   * Supported keys are BACKSPACE, DEL, ESC, CTR+D, LEFT ARROW, RIGHT ARROW, ENTER
   * Stores only printable characters
   *
   * @param buffer [out] string where the input will be stored - will be overwriten
   * @param prompt [in] string printed above the input line - should be shorter then the m_Width of the line
   * @return true - input confirmed -> ENTER key or newline
   * @return false - input closed prematurely -> CTR+D or ESC key
   */
  bool userInput(std::string &buffer, const std::string &prompt = "");

  /**
 * @brief Print system message to through ncurses
 *
 * @param message [in] message to be printed
 * @param originY [in] where to start on y axis
 * @param originX [in] where to start on x axis
 * @param tail [in] additional message
 */
  bool printMessage(const std::string &message, int originY = 1, int originX = 1, const std::string &tail = "") const;

  /**
   * @brief Draw empty box
   *
   * @return true - has allocated m_Window
   * @return false - has not allocated m_Window
   */
  bool drawBox() const;

  /**
   * @brief Resize the input field
   *
   * @param width [in] new width of the window  (-2 for borders for input field)
   * @param height [in] new height of the window
   * @param originX [in] x coordinate of the originof the window
   * @param originY [in] y coordinate of the origin of the window
   * @return true - success
   * @return false - wrong input
   */
  bool resize(int width, int height, int originX = 0, int originY = 0);

  /**
   * @brief Get the m_Height object
   *
   * @return int
   */
  int getHeight() const { return m_Height; }

protected:
  /**
   * @brief delete character form string
   * Warning: does not check input
   * @param buffer [in,out] string undergoing changes
   * @param iterator [in] position of the change
   * @param currLen [in] current length of the string (after the deletion)
   */
  void deleteFromBuffer(std::string &buffer, int iterator, int currLen);

  /**
   * @brief insert char in the string
   * Warning: does not check input
   * @param buffer [in,out] string undergoing changes
   * @param insertPoint [in] position of the insertion
   * @param currLen [in] current length of the string (before the insertion)
   * @param input [in] inputed character
   */
  void insertInBuffer(std::string &buffer, int insertPoint, int currLen, int input);

  /** working window */
  WINDOW *m_Window;
  /** width of the whole window */
  int m_Width;
  /** height of the whole window */
  int m_Height;
};

#endif