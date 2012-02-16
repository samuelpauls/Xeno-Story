/*
 * Rectangle.cpp
 *
 *  Created on: 2011-09-07
 *      Author: samuel
 */

#include <algorithm> // for std::max
#include "Rectangle.h"

using Game::Rectangle;

Rectangle::Rectangle() {
  x = 0;
  y = 0;
  w = 0;
  h = 0;
}

Rectangle::~Rectangle() {}

bool Rectangle::intersects (int x, int y) const throw() {
  return x >= this->x && x <= get_right() && y >= this->y && y <= get_top();
}

bool Rectangle::intersects (const Rectangle &rectangle) const throw() {
  return rectangle.get_right() >= x && rectangle.x <= get_right() &&
         rectangle.get_top() >= y && rectangle.y <= get_top();
}

Rectangle Rectangle::intersection (const Rectangle &rectangle) const throw() {
  if (intersects (rectangle)) {
    Rectangle r;
    r.x = std::max (x, rectangle.x);
    r.y = std::max (y, rectangle.y);
    int intersection_right = std::min (get_right(), rectangle.get_right());
    r.w = intersection_right - r.x;
    int intersection_top = std::min (get_top(), rectangle.get_top());
    r.h = intersection_top - r.y;
    return r;
  } else {
    return Rectangle();
  }
}
