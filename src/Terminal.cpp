/*
 * Terminal.cpp
 *
 *  Created on: 2012-01-22
 *      Author: samuel
 */

#include "Window.h"
#include "Terminal.h"

using Game::Terminal;

void Terminal::on_enter_pressed (void *terminal) throw() {
  Terminal *casted = (Terminal*)terminal;
  casted->world.queue_command (casted->input_textbox->get_text());
  casted->input_textbox->set_text ("");
}

Terminal::Terminal (World &world) throw(): world (world) {
  ui_manager = new UI::Manager();

  static const int PADDING = 4;

  UI::Frame *terminal_frame = new UI::Frame ("Terminal");
  Rectangle terminal_frame_rect;
  terminal_frame_rect.w = Window::get_width();
  terminal_frame_rect.h = (PADDING + 10 + PADDING) * 2
      + UI::Frame::PADDING + UI::Frame::TOP_PADDING; // TODO: Don't hard code the text line height.
  terminal_frame_rect.x = 0;
  terminal_frame_rect.y = Window::get_height() - terminal_frame_rect.h;
  terminal_frame->set_position (terminal_frame_rect);
  ui_manager->consume (terminal_frame);

  int grid_item_width = Window::get_width() - 2 * UI::Frame::PADDING;
  int grid_item_height = PADDING + 10 + PADDING;
  UI::GridLayout *grid =
      new UI::GridLayout (1, 2, 0, grid_item_width, grid_item_height);
  terminal_frame->consume (grid);

  output_label = new UI::Label();
  grid->consume (0, 0, output_label);

  input_textbox = new UI::TextBox();
  input_textbox->set_enter_callback (on_enter_pressed, this);
  grid->consume (0, 1, input_textbox);

  visible = false;
}

Terminal::~Terminal() throw() {
  delete ui_manager;
}

void Terminal::set_visible (bool visible) throw() {
  this->visible = visible;
  input_textbox->set_focus (visible);
}

void Terminal::draw() const throw() {
  if (visible) {
    ui_manager->draw();
  }
}
