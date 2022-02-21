#ifndef WINDOW_H_SENTRY
#define WINDOW_H_SENTRY

#include "glibmm/ustring.h"
#include "gtkmm/entry.h"
#include "gtkmm/headerbar.h"
#include "gtkmm/window.h"

const Glib::ustring PROJECT_NAME = Glib::ustring("Zosheet");
const Glib::ustring DEFAULT_CSS_PATH = "./data/styles/styles.css";

class MainWindow : public Gtk::Window {
  const Glib::ustring SEARCH_PLACEHOLDER = "Search";
  const Glib::ustring SEARCH_ICON_NAME = "edit-find";
  // const Glib::ustring DEFAULT_ICONS_PATH = "./data/icons/";

  Gtk::Entry search;
  Gtk::HeaderBar header_bar;

public:
  enum {
    DEFAULT_WIDTH = 700,
    DEFAULT_HEIGHT = 500,
  };
  MainWindow();

private:
  void set_css_provider(const Glib::ustring &path_to_file);
  void set_search_entry();
};

#endif // WINDOW_H_SENTRY
