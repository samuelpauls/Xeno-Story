/*
 * main.cpp
 *
 *  Created on: 2011-08-10
 *      Author: samuel
 */

#include <cstdio>
#include <cstdlib> // has EXIT_SUCCESS
#include <iostream>
#include <string>
#include <cassert>
#include <sstream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "UserSettings.h"
#include "Clock.h"
#include "Window.h"
#include "Animation.h"
#include "World.h"
#include "Controllers/Manager.h"
#include "Controllers/Player.h"
#include "Credits.h"
#include "Test.h"
#include "Font.h"
#include "UI/Manager.h"
#include "UI/Menu.h"
#include "UI/MenuTextItem.h"
#include "UserSettingsPanel.h"

using namespace Game;
using UI::Menu;
using UI::MenuTextItem;

static enum {MAIN_MENU, WORLD, CONTROLS, OPTIONS, CREDITS, EXIT} activity;
static World *world = NULL;
static Credits *credits = NULL;

// MENU

static Menu *menu;
static Game::Rectangle menu_rect;


// TITLE

static Font title_text (40);
static std::string title;
static GLfloat title_x, title_y;

static void set_title (std::string title) {
  ::title = title;
  title_x = menu_rect.x + (menu_rect.w - title_text.get_width (title)) / 2;
  title_y = 450.0F;
}


// TAKE AN ACTION

void display_menu (void *dummy) throw() {
  // If the world has started, change "Start Game" to "Resume Game".
  if (world) {
    MenuTextItem *mti = (MenuTextItem*)menu->get_widget (0);
    mti->set_text ("Resume Game");
  }

  set_title ("Xeno Story");
  activity = MAIN_MENU;
}

void start_game (void *dummy) throw() {
  if (world) {
    world->resume();
  } else {
    world = new World ("save");
  }

  activity = WORLD;
  menu->select_by_index (Menu::NOT_SET);
}

void display_controls (void *dummy) throw() {
  activity = CONTROLS;
  set_title ("Controls");
  menu->select_by_index (Menu::NOT_SET);
}

void display_options (void *dummy) throw() {
  activity = OPTIONS;
  set_title ("Options");
  menu->select_by_index (Menu::NOT_SET);
}

void display_credits (void *dummy) throw() {
  credits = new Credits();
  activity = CREDITS;
  menu->select_by_index (Menu::NOT_SET);
}

void exit_game (void *dummy) throw() {
  activity = EXIT;
}


int main (int argc, char **argv) {
  // TODO: Create a separate test suit executable instead of this code block.
  #ifndef NDEBUG
    testStringTokenizer();
  #endif

  UserSettings::load();

  Game::Window window;

  int r = glewInit();
  assert (r == GLEW_OK);

  // Setup an orthographic projection.
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D (0.0, Game::Window::get_width(), 0.0, Game::Window::get_height());

  // Enable color blending for transparent parts of PNG images.
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glMatrixMode (GL_MODELVIEW);

  Font disclaimer_text (10);
  Font controls_text (12);

  int fps;

  static const GLfloat PADDING = 10.0F; // in pixels

  #ifndef NDEBUG
    static const std::string FPS_LABEL = "FPS: ";

    // Create the FPS rectangle.
    static const std::string FPS_LONGEST_STR = FPS_LABEL + "000";
    Rectangle fps_rect;
    fps_rect.w = PADDING + disclaimer_text.get_width (FPS_LONGEST_STR) + PADDING;
    fps_rect.h = PADDING + disclaimer_text.get_line_height() + PADDING;
    fps_rect.x = Window::get_width() - fps_rect.w;
    fps_rect.y = Window::get_height() - fps_rect.h;
  #endif

  // Create the menu.
  static const int ITEM_WIDTH = 200, ITEM_HEIGHT = 50; // in pixels
  menu = new Menu (ITEM_WIDTH, ITEM_HEIGHT);
  menu->consume (new MenuTextItem ("Start Game", start_game, MenuTextItem::LARGE));
  menu->consume (new MenuTextItem ("Controls",  display_controls, MenuTextItem::LARGE));
  menu->consume (new MenuTextItem ("Options", display_options, MenuTextItem::LARGE));
  menu->consume (new MenuTextItem ("Credits", display_credits, MenuTextItem::LARGE));
  menu->consume (new MenuTextItem ("Exit", exit_game, MenuTextItem::LARGE));
  menu_rect.w = ITEM_WIDTH;
  menu_rect.h = ITEM_HEIGHT * menu->get_item_count();
  menu_rect.x = 400;
  menu_rect.y = (Window::get_height() - menu_rect.h) / 2;
  menu->set_position (menu_rect);
  UI::Manager menu_ui_manager;
  menu_ui_manager.consume (menu);

  display_menu (NULL);

  UserSettingsPanel *user_settings_widget = NULL;

  // Main game loop.
  const int DESIRED_FPS = 60;
  static const unsigned int DESIRED_FRAME_DURATION = 1000 / DESIRED_FPS;
  Clock clock;
  do {
    unsigned int start_tick = clock.getElapsedMS();

    Controllers::Manager::update (window);

    // Update, draw, and eventually terminate the world.
    if (activity == WORLD) {
      if (Controllers::Manager::is_escape_pressed()) {
        display_menu (NULL);
      } else {
        world->update (start_tick);
        if (world->get_status() == World::PLAYER_WON) {
          delete world;
          world = NULL;
          display_credits (NULL);
        } else if (world->get_status() == World::PLAYER_LOST) {
          delete world;
          world = NULL;
          start_game (NULL);
        } else if (world->get_status() == World::RUNNING) {
          world->draw();
        }
      }
    } else if (activity == CREDITS) {
      credits->update (start_tick);
      if (credits->is_done() || Controllers::Manager::is_escape_pressed()) {
        delete credits;
        credits = NULL;
        display_menu (NULL);
      } else {
        credits->draw (window);
      }
    } else {
      // Draw the backdrop.
      if (world) {
        world->draw();

        // Blacken the world by 80%.
        glColor4f (0.0F, 0.0F, 0.0F, 0.8F);
        glPushMatrix();
        glLoadIdentity();
        glRecti (0, 0, Window::get_width(), Window::get_height());
        glPopMatrix();
      } else {
        // Draw the background.
        glPushMatrix();
        glLoadIdentity();
        static Animation background ("ui/main_menu.png");
        static const GLfloat BG_SCALE =
            (GLfloat)Game::Window::get_width() / background.get_width();
        glScalef (BG_SCALE, BG_SCALE, 0.0F);
        background.draw();
        glPopMatrix();

        glMatrixMode (GL_MODELVIEW);
        glColor3ub (64, 40, 196); // dark purple

        // Draw the disclaimer.
        static const std::string INFO_STR = "Contributions copyright (2011) by their "
            "respective authors listed in the credits.  Not a finished work.";
        static const int INFO_X =
            (Game::Window::get_width() - disclaimer_text.get_width (INFO_STR)) / 2;
        glLoadIdentity();
        glTranslatef (INFO_X, PADDING, 0.0F);
        disclaimer_text.draw (INFO_STR);

        // Draw the version.
        static const std::string VER_STR = "v 0.7";
        static const int VER_X = Window::get_width() - PADDING - disclaimer_text.get_width (VER_STR);
        glLoadIdentity();
        glTranslatef (VER_X, PADDING, 0.0F);
        disclaimer_text.draw (VER_STR);
      }

      // Draw the title.
      glPushMatrix();
      glLoadIdentity();
      glTranslatef (title_x, title_y, 0.0F);
      glColor3ub (64, 40, 196); // dark purple
      title_text.draw (title);
      glPopMatrix();

      if (activity == CONTROLS) {
        if (Controllers::Manager::is_escape_pressed()) {
          display_menu (NULL);
        } else {
          // Draw the controls.
          glPushMatrix();
          glLoadIdentity();
          glTranslatef (menu_rect.x,
                        menu_rect.get_top() - controls_text.get_line_height(),
                        0.0F);
          glColor3ub (156, 120, 252); // light purple
          controls_text.draw (
              "Arrows: Move, aim\n"
              "z: fire weapon, select menu item\n"
              "a, s: Select weapon\n"
              "x: Jump\n"
              "Down: Use door, talk, etc\n"
              "Esc: Back\n"
              "\n"
              "F1: Editor\n"
              "Mouse buttons 3, 4: Select map layer\n"
              "Mouse wheel: Select brush\n"
              "Left click: Select or use brush\n"
              "Right click: Copy brush\n"
              "\n"
              "F2 Terminal");
          glPopMatrix();
        }
      } else if (activity == OPTIONS) {
        if (!user_settings_widget) {
          user_settings_widget = new UserSettingsPanel();
        }
        user_settings_widget->update (start_tick);
        user_settings_widget->draw();
        if (Controllers::Manager::is_escape_pressed()) {
          delete user_settings_widget;
          user_settings_widget = NULL;
          display_menu (NULL);
        }
      } else if (activity == MAIN_MENU) {
        // Select a menu item by hot key.
        static Controllers::Player pc;
        if (pc.is_up_pressed()) {
          menu->decrement_highlight();
        }
        if (pc.is_down_pressed()) {
          menu->increment_highlight();
        }
        if (pc.is_weapon_fire_pressed()) {
          menu->select_highlighted();
        }

        menu_ui_manager.update (start_tick);
        menu_ui_manager.draw();
      }
    }

    // Draw FPS if compiled for debugging.
    #ifndef NDEBUG
      // Create the FPS string.
      std::stringstream fps_ss;
      fps_ss << FPS_LABEL << fps;
      std::string fps_str = fps_ss.str();

      // Move to the FPS label's location.
      glLoadIdentity();
      glTranslatef (fps_rect.x, fps_rect.y, 0.0F);

      // Draw the FPS label's background.
      glColor4f (0.0F, 0.0F, 0.0F, 0.8F); // 80% black
      glRecti (0, 0, fps_rect.w, fps_rect.h);

      // Draw the FPS label.
      glTranslatef (PADDING, PADDING, 0.0F);
      glColor3ub (64, 40, 196); // dark purple
      disclaimer_text.draw (fps_str);
    #endif

    window.refresh();

    unsigned int frame_duration = clock.getElapsedMS() - start_tick;
    if (frame_duration >= DESIRED_FRAME_DURATION) {
      fps = 1000 / frame_duration;
    } else { // if the frame duration is less than desired
      fps = DESIRED_FPS;
      clock.sleep (DESIRED_FRAME_DURATION - frame_duration);
    }
  } while (activity != EXIT && !Controllers::Manager::is_app_closed());

  delete world;
  delete credits;
  delete user_settings_widget;

  return EXIT_SUCCESS;
}
