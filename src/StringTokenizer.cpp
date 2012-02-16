/*
 * StringTokenizer.cpp
 *
 *  Created on: 2011-08-22
 *      Author: samuel
 */

#include <cstdlib>
#include "StringTokenizer.h"

using Game::StringTokenizer;

StringTokenizer::StringTokenizer (const std::string &string) throw() {
  this->string = string;
  delimiter = ' ';
}

StringTokenizer::~StringTokenizer() throw() {}

std::string StringTokenizer::get_next_string() {
  int token_start;
  int token_end;
  std::string token;
  size_t non_white_space_start;

  non_white_space_start = string.find_first_not_of (delimiter);

  // Determine the start and end of this token.
  char first_char = string.at (non_white_space_start);
  if (first_char == '"' || first_char == '\'') {
    token_start = non_white_space_start + 1;
    token_end = string.find (first_char, token_start);
  } else {
    token_start = non_white_space_start;
    token_end = string.find (delimiter, token_start);
    if (token_end == (int)std::string::npos) { // not found
      token_end = string.length();

      // Remove control characters from the token that's to be returned.
      while (!isprint (string[token_end - 1])) {
        --token_end;
      }
    }
  }

  token = string.substr (token_start, token_end - token_start);

  // Remove this token.
  int next_token_start = string.find_first_not_of (delimiter, token_end + 1);
  if (next_token_start == (int)std::string::npos) { // not found
    next_token_start = string.length();
  }
  string.erase (0, next_token_start);

  return token;
}

int StringTokenizer::get_next_integer() {
  return std::atoi (get_next_string().c_str());
}

double StringTokenizer::get_next_double() {
  return std::atof (get_next_string().c_str());
}

char StringTokenizer::get_next_character() {
  return get_next_string().at(0);
}
