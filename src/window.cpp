#include "window.h"
#include "gdkmm/display.h"
#include "glibmm/ustring.h"
#include "gtkmm/cssprovider.h"
#include "gtkmm/styleprovider.h"

MainWindow::MainWindow() : Window(), search(), header_bar()
{
  set_title(PROJECT_NAME);
  set_default_size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  set_size_request(DEFAULT_WIDTH, DEFAULT_HEIGHT);
  set_titlebar(header_bar);
  this->set_search_entry();
  this->set_css_provider(DEFAULT_CSS_PATH); // TODO:
  // get_style_context()->add_class("rounded"); // Add round corners ot window
}

void MainWindow::set_css_provider(const Glib::ustring &path_to_file)
{
  auto css = Gtk::CssProvider::create();
  css->load_from_path(path_to_file);
  auto screen = Gdk::Display::get_default();
  auto ctx = this->get_style_context();
  ctx->add_provider_for_display(screen, css, 1);
}

void MainWindow::set_search_entry()
{
  search.set_placeholder_text(SEARCH_PLACEHOLDER);
  search.set_size_request(DEFAULT_WIDTH / 3 - 5, -1);
  search.set_icon_from_icon_name(SEARCH_ICON_NAME);
  header_bar.pack_start(search);
}
