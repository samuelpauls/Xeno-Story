/*
 * Rectangle.h
 *
 *  Created on: 2011-09-07
 *      Author: samuel
 */

#ifndef RECTANGLE_H_
#define RECTANGLE_H_

namespace Game {
  // A geometric rectangle.
  class Rectangle {
  public:
    int x, y;
    int w, h;

    Rectangle();
    virtual ~Rectangle();

    int get_right() const throw() { return x + w; }
    int get_top() const throw() { return y + h; }

    // Returns true if the rectangle intersects the x and y point, false
    // otherwise.
    bool intersects (int x, int y) const throw();

    // Returns true if the rectangles intersect or false otherwise.
    bool intersects (const Rectangle &rectangle) const throw();

    // Returns the rectangle that overlaps this and the provided rectangle.
    Rectangle intersection (const Rectangle &rectangle) const throw();
  };
}

#endif /* RECTANGLE_H_ */
