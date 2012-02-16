/*
 * Window.h
 *
 *  Created on: 2011-08-10
 *      Author: samuel
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <SFML/Graphics.hpp>
#include "TileLayer/TileLayout.h"

namespace Game {
  namespace Controllers {
    class Manager;
  }

  // Responsible for displaying graphics to the user.
  class Window {
    friend class Controllers::Manager;
    friend class Font;

  private:
    // By making the window size a multiple of the tile size, we ensure that
    // there are at most one extra row or column of tiles on the screen at once.
    // (If we didn't use a multiple of the tile size and the tiles are centered
    // in the viewport, we could have a bit of an extra tile on either end, that
    // is two extra rows or columns of tiles.  This would require a change in
    // the pixel to tile rectangle code.)
    static const int WIDTH = 800;
    static const int HEIGHT = 600;

    static sf::RenderWindow *app;

  public:
    Window() throw();
    virtual ~Window() throw();

    static int get_width() { return WIDTH; }
    static int get_height() { return HEIGHT; }
    float get_scale_x() const throw() { return (float)app->GetWidth() / WIDTH; }
    float get_scale_y() const throw() {
      return (float)app->GetHeight() / HEIGHT;
    }

    // Update the screen with the contents of this window.
    void refresh() throw();
  }; // class Window
} // namespace Game

#endif // WINDOW_H
