#ifndef CDIRECTORY_H
#define CDIRECTORY_H

#include <string>

/**
 * @brief Class for viewing files in directory
 * 
 */
class CDirectory
{
public:
/**
 * @brief List files in a directory
 * 
 * @param path [in] path to directory to be listed
 * @param header [in] header of the listed files
 * @param originX [in] x coordinate of the origin of the table
 * @param originY [in] y coordinate of the origin of the table
 * @return true - listed successfully
 * @return false - erroer occurred (wrong path)
 */
  bool listFiles(const std::string &path, const std::string &header = "", int originX = 0, int originY = 0) const;
};

#endif