#ifndef CPARSERMAP_H
#define CPARSERMAP_H

#include "CParser.h"
#include "CCoordinates.h"
#include "CTower.h"

#include <string>
#include <memory>

/**
 * @brief Class containing functions for parsing CMap data files
 *
 */
class CParserMap : public CParser
{
public:
  /**
   * @brief Parse coordinates of Start and End
   *
   * @param coordinates [out] parsed coordinates of the point
   * @param axisX [in] x coordinate
   * @param axisY [in] y coordinate
   * @param width [in] width of the map
   * @param height [in] height of the map
   * @return true - parsed successfully
   * @return false - invalid input (axes)
   */
  bool parseDistinctPoint(CCoordinates &coordinates, const std::string &axisX, const std::string &axisY, int width, int height);

  /**
   * @brief Get CTower form its symbol and coordinates
   *
   * @param symbol [in] symbol of the to-be CTower
   * @param pointer [out] pointer to the newly created CTower
   * @param coordinates [out] parsed coordinates of the CTower
   * @param axisX [in] x coordinate
   * @param axisY [in] y coordinate
   * @param width [in] width of the map
   * @param height [in] height of the map
   * @return true - parsed successfully
   * @return false - invalid input (axes/symbol)
   */
  bool parseCTower(const std::string &symbol, std::shared_ptr<CTower> &pointer,
                   CCoordinates &coordinates, const std::string &axisX, const std::string &axisY, int width, int height);

protected:
  /**
   * @brief Parse coordinates of a point in the map
   *
   * @param coordinates [out] parsed coordinates of the point
   * @param axisX [in] x coordinate
   * @param axisY [in] y coordinate
   * @param width [in] width of the map
   * @param height [in] height of the map
   * @return true - parsed successfully
   * @return false - invalid input (axes)
   */
  bool parsePoint(CCoordinates &coordinates, const std::string &axisX, const std::string &axisY, int width, int height);
};

#endif