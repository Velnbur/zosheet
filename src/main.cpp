#include "glibmm/ustring.h"
#include "gtkmm/application.h"
#include "gtkmm/box.h"
#include "gtkmm/enums.h"
#include "gtkmm/paned.h"
#include "widgets.h"
#include "window.h"

int main(int argc, char *argv[])
{
  auto app = Gtk::Application::create("com.github.Velnbur.zosheet");
  auto window = new MainWindow();
  app->signal_activate().connect([app, window]() { app->add_window(*window); });

  auto box = Gtk::Paned(Gtk::Orientation::HORIZONTAL);
  box.set_position(MainWindow::DEFAULT_WIDTH / 3);

  auto notes_box = NotesListBox();
  for (int i = 0; i < 30; i++) {
    NoteListItem note_item(
        Glib::ustring("Title That is a little bit more long"),
        Glib::ustring("Some text in here to checl width"));
    notes_box.add_item(note_item);
  }

  box.set_start_child(notes_box);

  NoteEdit note_edit;
  box.set_end_child(note_edit);

  window->set_child(box);
  window->show();
  return app->run(argc, argv);
}
