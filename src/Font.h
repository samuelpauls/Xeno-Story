/*
 * Font.h
 *
 *  Created on: 2011-11-03
 *      Author: samuel
 */

#ifndef FONT_H
#define FONT_H

#include <string>
#include <SFML/Graphics.hpp>

namespace Game {
  // Minimal text drawing for OpenGL.
  class Font {
  private:
    mutable sf::String drawable_string;

  public:
    Font (int size) throw();

    virtual ~Font() throw();

    // in pixels
    int get_line_height() const throw() { return drawable_string.GetSize(); }
    int get_width (const std::string &text) const throw();
    int get_height (const std::string &text) const throw();

    // Draws text with the origin at the bottom-left of the first line.
    void draw (const std::string &text) const throw();
  }; // class Font
} // namespace Game

#endif // FONT_H
