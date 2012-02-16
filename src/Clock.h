/*
 * Clock.h
 *
 *  Created on: 2011-09-08
 *      Author: samuel
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <SFML/System/Clock.hpp>

namespace Game {
  class Clock {
  private:
    sf::Clock clock;
  
  public:
    Clock();
    virtual ~Clock();

    // Pause execution for ms milliseconds.
    void sleep (int ms) const throw();

    // Returns the number of milliseconds that have elapsed.
    int getElapsedMS() const throw();
  };
}

#endif // CLOCK_H
