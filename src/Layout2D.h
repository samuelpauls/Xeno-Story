/*
 * Layout2D.h
 *
 *  Created on: 2012-01-11
 *      Author: samuel
 */

#ifndef LAYOUT2D_H
#define LAYOUT2D_H

#include <cassert>
#include <vector>

namespace Game {
  // A 2D layout of objects.
  template<class T> class Layout2D {
  private:
    std::vector< std::vector<T> > objects; // stored by columns, then rows
  public:
    Layout2D (int columns, int rows, T initial_value) throw();
    virtual ~Layout2D() throw();

    void set (int column, int row, T object) throw();
    T get (int column, int row) const throw();

    int get_column_count() const throw() { return objects.size(); }
    int get_row_count() const throw() { return objects.front().size(); }
  };
} // namespace Game


// Implementation of a template must be in the same file.

using Game::Layout2D;

template<class T> Layout2D<T>::Layout2D (int columns, int rows, T initial_value) throw() {
  objects.insert (objects.begin(), columns, std::vector<T>());

  // For each column...
  for (typename std::vector< std::vector<T> >::iterator c = objects.begin();
       c != objects.end();
       ++c) {
    c->insert (c->begin(), rows, initial_value);
  } // for each column

}

template<class T> Layout2D<T>::~Layout2D() throw() {}

template<class T>
void Layout2D<T>::set (int column, int row, T object) throw() {
  assert (column >= 0 && column < get_column_count());
  assert (row >= 0 && row < get_row_count());
  objects[column][row] = object;
}

template<class T> T Layout2D<T>::get (int column, int row) const throw() {
  assert (column >= 0 && column < get_column_count());
  assert (row >= 0 && row < get_row_count());
  return objects[column][row];
}


#endif // LAYOUT2D_H
