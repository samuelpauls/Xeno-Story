/*
 * Manager.cpp
 *
 *  Created on: 2011-08-20
 *      Author: samuel
 */

#include <SFML/Window/Event.hpp>
#include "Player.h"
#include "Manager.h"

using Game::Controllers::Manager;

bool Manager::escape_pressed = false;
bool Manager::app_closed = false;
Game::Controllers::UI Manager::ui_controller;
Game::Controllers::Editor Manager::editor_controller;

bool Manager::terminal_toggle_pressed = false;
bool Manager::terminal_open = false;
int Manager::cursor_index_change = 0;
std::string Manager::keyboard_buffer;

Manager::Manager() throw() {}

Manager::~Manager() throw() {}

void Manager::update (const Window &window) throw() {
  // The following inputs are only active for the update immediately following a
  // physical press.
  terminal_toggle_pressed = false;
  Player::weapon_previous = false;
  Player::weapon_next = false;
  editor_controller.toggle_pressed = false;
  editor_controller.previous_brush = false;
  editor_controller.next_brush = false;
  editor_controller.previous_layer = false;
  editor_controller.next_layer = false;

  // Loop through all user input events in one place, instead of in each
  // controller.  This allows us to process all the events without having to put
  // them back on the event queue and worry about needing to selectively ignore
  // them later after a given controller has processed all the events it's
  // interested in.
  sf::Event event;
  bool pressed;
  while (Window::app->GetEvent (event)) {
    switch (event.Type) {
    case sf::Event::KeyPressed:
      // Modify the cursor index at the OSes rate for a pressed arrow.
      if (terminal_open) {
        if (event.Key.Code == sf::Key::Left) {
          --cursor_index_change;
        } else if (event.Key.Code == sf::Key::Right) {
          ++cursor_index_change;
        }
      }

      // fall through to next case
    case sf::Event::KeyReleased:
      pressed = event.Type == sf::Event::KeyPressed;

      // Update the controller's state.
      switch (event.Key.Code) {
      case sf::Key::Escape:
        escape_pressed = pressed;
        break;
      case sf::Key::Left:
        Player::left_pressed = pressed;
        editor_controller.scroll_left_pressed = pressed;
        break;
      case sf::Key::Right:
        Player::right_pressed = pressed;
        editor_controller.scroll_right_pressed = pressed;
        break;
      case sf::Key::Up:
        Player::up_pressed = pressed;
        editor_controller.scroll_up_pressed = pressed;
        break;
      case sf::Key::Down:
        Player::down_pressed = pressed;
        editor_controller.scroll_down_pressed = pressed;
        break;
      case sf::Key::X:
        Player::jump_pressed = pressed;
        break;
      case sf::Key::A:
        Player::weapon_previous = pressed;
        break;
      case sf::Key::S:
        Player::weapon_next = pressed;
        break;
      case sf::Key::Z:
        Player::weapon_fire = pressed;
        break;
      case sf::Key::F1:
        editor_controller.toggle_pressed = pressed;
        break;
      case sf::Key::F2:
        terminal_toggle_pressed = pressed;
        break;
      case sf::Key::Delete:
        editor_controller.delete_pressed = pressed;
        break;
      default:
        break;
      }

      break;
    case sf::Event::TextEntered:
      if (terminal_open) {
        if (event.Text.Unicode == 13) {
          keyboard_buffer += '\n';
        } else {
          keyboard_buffer += event.Text.Unicode;
        }
      }

      break;
    case sf::Event::MouseButtonPressed:
    case sf::Event::MouseButtonReleased:
      pressed = event.Type == sf::Event::MouseButtonPressed;

      switch (event.MouseButton.Button) {
      case sf::Mouse::Left:
        ui_controller.primary_down = pressed;
        break;
      case sf::Mouse::Right:
        ui_controller.secondary_down = pressed;
        break;
      case sf::Mouse::XButton1:
        editor_controller.next_layer = pressed;
        break;
      case sf::Mouse::XButton2:
        editor_controller.previous_layer = pressed;
        break;
      default:
        break;
      }
      break;
    case sf::Event::MouseWheelMoved:
      if (event.MouseWheel.Delta > 0) {
        editor_controller.previous_brush = true;
      } else if (event.MouseWheel.Delta < 0){
        editor_controller.next_brush = true;
      }
      break;
    case sf::Event::MouseMoved:
      ui_controller.x = event.MouseMove.X / window.get_scale_x();
      ui_controller.y =
          Window::get_height() - event.MouseMove.Y / window.get_scale_y();
      break;
    case sf::Event::Closed:
      app_closed = true;
      break;
    case sf::Event::JoyButtonPressed:
    case sf::Event::JoyButtonReleased:
      pressed = event.Type == sf::Event::JoyButtonPressed;
      switch (event.JoyButton.Button) {
      case 0:
        Player::jump_pressed = pressed;
        break;
      case 1:
        Player::weapon_fire = pressed;
        break;
      case 2:
        Player::weapon_next = pressed;
        break;
      case 3:
        Player::weapon_previous = pressed;
        break;
      } // joy button
      break;
    case sf::Event::JoyMoved:
      static const float NOTICEABLE_POSITION = 50.0F;

      if (event.JoyMove.Axis == sf::Joy::AxisX) {
        if (event.JoyMove.Position <= -NOTICEABLE_POSITION) {
          Player::left_pressed = true;
        } else {
          Player::left_pressed = false;
        }
        if (event.JoyMove.Position >= NOTICEABLE_POSITION) {
          Player::right_pressed = true;
        } else {
          Player::right_pressed = false;
        }
      }

      if (event.JoyMove.Axis == sf::Joy::AxisY) {
        if (event.JoyMove.Position <= -NOTICEABLE_POSITION) {
          Player::up_pressed = true;
        } else {
          Player::up_pressed = false;
        }
        if (event.JoyMove.Position >= NOTICEABLE_POSITION) {
          Player::down_pressed = true;
        } else {
          Player::down_pressed = false;
        }
      }

      break;
    default:
      break;
    } // event type
  } // while there are events
}

void Manager::open_terminal() throw() {
  terminal_open = true;
}

void Manager::close_terminal() throw() {
  terminal_open = false;
}

int Manager::get_cursor_index_change() throw() {
  int r = cursor_index_change;
  cursor_index_change = 0;
  return r;
}

std::string Manager::get_keyboard_buffer() throw() {
  std::string r = keyboard_buffer;
  keyboard_buffer = "";
  return r;
}
