/*
 * FileSystem.cpp
 *
 *  Created on: 2011-10-23
 *      Author: samuel
 */

#include <cstdlib> // for std::system

#if defined UNIX
  #include <sys/stat.h> // for mkdir
#elif defined WINDOWS
  #include <windows.h> // for CreateDirectory
#endif

#include "FileSystem.h"

void Game::FileSystem::remove (const std::string &filename) throw() {
  std::string rm_command;
  #if defined UNIX
    // -r for recursive
    // -f for force (aka don't complain if the path doesn't exist)
    rm_command = "rm -rf " + filename;
  #elif defined WINDOWS
    // /s for delete subdirectories and files within the directory
    // /q for delete quietly without confirmation
    rm_command = "rmdir /s /q " + filename;
  #endif
  std::system (rm_command.c_str());
}

void Game::FileSystem::create_path (const std::string &path) throw() {
  #if defined UNIX
    mkdir (path.c_str(), S_IRWXU);
  #elif defined WINDOWS
    CreateDirectory (path.c_str(), NULL);
  #endif
}

bool Game::FileSystem::exists (const std::string &path) throw() {
  #if defined UNIX
    struct stat attribs;
    static const int EXISTS = 0;
    return stat (path.c_str(), &attribs) == EXISTS;
  #elif defined WINDOWS
    return GetFileAttributes (path.c_str()) != 0xFFFFFFFF;
  #endif
}
