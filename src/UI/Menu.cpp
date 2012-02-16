/*
 * Menu.cpp
 *
 *  Created on: 2011-09-18
 *      Author: samuel
 */

#include "Widget.h"
#include "Menu.h"

using Game::UI::Menu;

Menu::Menu (int item_width, int item_height, int padding) throw():
           PaletteLayout (item_width, item_height, padding) {
  change_sound = new Audio ("ui/menu_select.ogg");
  highlight_index = NOT_SET;
  selection_index = NOT_SET;
}

Menu::~Menu() throw() {
  delete change_sound;
}

Game::Rectangle Menu::get_priority_rectangle() const throw() {
  Widget *widget = get_selected_widget();
  if (widget) {
    Rectangle priority_rect;
    Rectangle widget_rect = widget->get_position();
    priority_rect.x = location.x + widget_rect.x - item_padding;
    priority_rect.y = location.y + widget_rect.y - item_padding;
    priority_rect.w = widget_rect.w + 2 * item_padding;
    priority_rect.h = widget_rect.h + 2 * item_padding;
    return priority_rect;
  } else {
    return location;
  }
}

bool Menu::select_by_pixel (int window_x, int window_y) throw() {
  for (int i = 0; i < get_item_count(); ++i) {
    if (selection_index != i) {
      Widget *w = get_widget (i);
      if (w->intersects(window_x, window_y)) {
        if (selection_index != NOT_SET) {
          get_selected_widget()->set_selected (false);
        }
        selection_index = i;
        get_selected_widget()->set_selected (true);
        return true; // menu item selected due to this method call
      }
    }
  }

  return false; // no menu item selected due to this method call
}

void Menu::increment_selection() throw() {
  if (selection_index == NOT_SET) {
    selection_index = 0;
  } else {
    get_selected_widget()->set_selected (false);

    // Increment the selection.
    ++selection_index;
    if (selection_index == get_item_count()) {
      selection_index = 0;
    }
  }
  get_selected_widget()->set_selected (true);
  change_sound->play();
}

void Menu::decrement_selection() throw() {
  if (selection_index == NOT_SET) {
    selection_index = get_item_count() - 1;
  } else {
    get_selected_widget()->set_selected (false);

    // Decrement the selection.
    if (selection_index == 0) {
      selection_index = get_item_count() - 1;
    } else {
      --selection_index;
    }
  }
  get_selected_widget()->set_selected (true);
  change_sound->play();
}

void Menu::increment_highlight() throw() {
  if (highlight_index == NOT_SET) {
    highlight_index = 0;
  } else {
    get_highlighted_widget()->set_highlighted (false);

    // Increment the highlight.
    ++highlight_index;
    if (highlight_index == get_item_count()) {
      highlight_index = 0;
    }
  }
  get_highlighted_widget()->set_highlighted (true);
  change_sound->play();
}

void Menu::decrement_highlight() throw() {
  if (highlight_index == NOT_SET) {
    highlight_index = get_item_count() - 1;
  } else {
    get_highlighted_widget()->set_highlighted (false);

    // Decrement the highlight.
    if (highlight_index == 0) {
      highlight_index = get_item_count() - 1;
    } else {
      --highlight_index;
    }
  }
  get_highlighted_widget()->set_highlighted (true);
  change_sound->play();
}

void Menu::select_highlighted() throw() {
  // Select only the highlighted item.
  if (selection_index != highlight_index) {
    if (selection_index != NOT_SET) {
      get_selected_widget()->set_selected (false);
    }
    selection_index = highlight_index;
    get_selected_widget()->set_selected (true);

    change_sound->play();
  }

  // Unhighlight the highlighted widget.
  get_highlighted_widget()->set_highlighted (false);
  highlight_index = NOT_SET;
}

bool Menu::select_by_index (int index) throw() {
  if (selection_index == index) {
    return false; // selection not changed
  } else {
    if (selection_index != NOT_SET) {
      get_selected_widget()->set_selected (false);
    }
    selection_index = index;
    if (index != NOT_SET) {
      get_selected_widget()->set_selected (true);
      change_sound->play();
    }
    return true; // selection changed
  }
}

Game::UI::MenuItem *Menu::get_highlighted_widget() const throw() {
  if (highlight_index == NOT_SET) {
    return NULL;
  } else {
    return (MenuItem*)get_widget (highlight_index);
  }
}

Game::UI::MenuItem *Menu::get_selected_widget() const throw() {
  if (selection_index == NOT_SET) {
    return NULL;
  } else {
    return (MenuItem*)get_widget (selection_index);
  }
}

Game::UI::Widget *Menu::mouse_move (int x, int y) throw() {
  // Highlight the menu item that the mouse is hovering over.
  for (int i = 0; i < get_item_count(); ++i) {
    if (highlight_index != i) {
      Widget *w = get_widget (i);
      int x_relative_to_menu = x - location.x;
      int y_relative_to_menu = y - location.y;
      if (w->get_position().intersects (x_relative_to_menu,
                                        y_relative_to_menu)) {
        highlight_index = i;
        change_sound->play();
        break;
      }
    }
  }

  // MenuItems automatically set their highlighting due to mouse events.
  Widget *hovered_widget = Layout::mouse_move (x, y);

  return hovered_widget;
}

void Menu::mouse_primary_pressed (int x, int y) throw() {
  // Select the menu item that the mouse is clicking over.
  for (int i = 0; i < get_item_count(); ++i) {
    if (selection_index != i) {
      Widget *w = get_widget (i);
      int x_relative_to_menu = x - location.x;
      int y_relative_to_menu = y - location.y;
      if (w->get_position().intersects (x_relative_to_menu,
                                        y_relative_to_menu)) {
        if (selection_index != NOT_SET) {
          get_selected_widget()->set_selected (false);
        }
        selection_index = i;
        change_sound->play();
        break;
      }
    }
  }

  // MenuItems automatically select themselves when clicked.
  Layout::mouse_primary_pressed (x, y);
}

void Menu::mouse_exit() throw() {
  highlight_index = NOT_SET;
}
