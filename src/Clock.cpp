/*
 * Clock.cpp
 *
 *  Created on: 2011-09-08
 *      Author: samuel
 */

#include <SFML/System/Sleep.hpp>
#include "Clock.h"

using Game::Clock;

Clock::Clock() {}

Clock::~Clock() {}

void Clock::sleep (int ms) const throw() {
  if (ms > 0) {
    sf::Sleep (ms / 1000.0);
  }
}

int Clock::getElapsedMS() const throw() {
  return clock.GetElapsedTime() * 1000;
}
