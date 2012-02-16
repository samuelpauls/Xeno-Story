/*
 * UI.cpp
 *
 *  Created on: 2012-01-04
 *      Author: samuel
 */

#include <cassert>
#include "UI.h"

using Game::Controllers::UI;

UI::UI() throw() {
  x = 0;
  y = 0;

  primary_down = false;
  secondary_down = false;
}

UI::~UI() throw() {}
