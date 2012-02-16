/*
 * StringTokenizer.h
 *
 *  Created on: 2011-08-22
 *      Author: samuel
 */

#ifndef STRINGTOKENIZER_H
#define STRINGTOKENIZER_H

#include <string>

namespace Game {
  // Parses a string for easy configuration from files.
  // C++'s standard library lacks a string tokenizer.
  class StringTokenizer {
  private:
    std::string string;
    char delimiter;

  public:
    StringTokenizer (const std::string &string) throw();
    virtual ~StringTokenizer() throw();

    void set_delimiter (char delimiter) throw() {
      this->delimiter = delimiter;
    }

    std::string get_next_string();
    int get_next_integer();
    double get_next_double();
    char get_next_character();

    bool has_more_tokens() const throw() { return string.size() > 0; }

    // Returns the portion of the string that hasn't been used up as tokens.
    std::string get_remaining_string() const throw() { return string; }
  };
}

#endif // STRINGTOKENIZER_H
