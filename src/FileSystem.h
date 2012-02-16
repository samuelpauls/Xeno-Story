/*
 * FileSystem.h
 *
 *  Created on: 2011-10-23
 *      Author: samuel
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>

namespace Game {
  namespace FileSystem {
    // Delete a file or directory (including everything in the directory).
    void remove (const std::string &filename) throw();

    // Create a path if it doesn't already exist.
    void create_path (const std::string &path) throw();

    // Returns true if a file or path exists, false otherwise.
    bool exists (const std::string &path) throw();
  } // namespace FileSystem
} // namespace Game

#endif // FILESYSTEM_H
