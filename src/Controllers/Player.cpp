/*
 * Player.cpp
 *
 *  Created on: 2011-08-11
 *      Author: samuel
 */

#include "Player.h"

using Game::Controllers::Player;

bool Player::left_pressed = false;
bool Player::right_pressed = false;
bool Player::up_pressed = false;
bool Player::down_pressed = false;
bool Player::jump_pressed = false;

bool Player::weapon_previous = false;
bool Player::weapon_next = false;
bool Player::weapon_fire = false;

Player::Player() throw() {}

Player::~Player() throw() {}
