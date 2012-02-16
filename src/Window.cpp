/*
 * Window.cpp
 *
 *  Created on: 2011-08-10
 *      Author: samuel
 */

#include <GL/gl.h>
#include "Window.h"

using Game::Window;

sf::RenderWindow *Window::app;

Window::Window() throw() {
  static const int DEPTH = 32;
  app = new sf::RenderWindow (sf::VideoMode (WIDTH, HEIGHT, DEPTH), "Xeno Story");
  app->UseVerticalSync (true); // set to false by default
}

Window::~Window() throw() {
  app->Close();
}

void Window::refresh() throw() {
  glFlush(); // finish commands
  app->Display(); // swap buffers
}
