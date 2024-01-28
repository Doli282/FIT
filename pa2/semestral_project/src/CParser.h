#ifndef CPARSER_H
#define CPARSER_H

#include <string>

/**
 * @brief Class containing methods for data file parsing
 *
 */
class CParser
{
public:
  /**
   * @brief Construct a new CParser object
   *
   */
  CParser();

  /**
   * @brief Check if filename has desired prefix (case insensitive)
   *
   * @param filename [in] name of the file
   * @param prefix [in] desired prefix
   * @return true - filename has desired prefix
   * @return false - filename has different prefix
   */
  bool nameMatch(const std::string &filename, const std::string &prefix) const;

  /**
   * @brief Convert number from input string to positive integer (0 included)
   *
   * @param parameter [out] here the number will be stored
   * @param input [in] string with input
   * @return true - number successfully parsed
   * @return false - there was not a positive integer in the input string
   */
  bool parseNumber(int &parameter, const std::string &input);

  /**
   * @brief test if methods in the calss are working properly
   *
   * @return true
   * @return false
   */
  bool asserts();
};

#endif