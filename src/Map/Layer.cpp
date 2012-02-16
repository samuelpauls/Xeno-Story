/*
 * Layer.cpp
 *
 *  Created on: 2011-09-01
 *      Author: samuel
 */

#include "Layer.h"

using Game::Map::Layer;

Layer::Layer (const std::string &name) throw() {
  this->name = name;
}

Layer::~Layer() throw() {}
