#ifndef WIDGETS_H_SENTRY
#define WIDGETS_H_SENTRY
#include <bits/types/time_t.h>

#include "glibmm/ustring.h"
#include "gtkmm/box.h"
#include "gtkmm/label.h"
#include "gtkmm/listbox.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/textview.h"
#include "objects.h"
#include "parser.h"

/*
**  Class of items that contains notes info to display in NotesList
**
**  Looks smth like this:
**  *----------------------------*
**  | Title text      12.03.2022 |
**  | Some text from note...     |
**  *----------------------------*
*/
class NoteListItem : public Gtk::Box {
  Gtk::Label title, text, date;
  Gtk::Box h_box, v_box;

public:
  NoteListItem(const Glib::ustring &name, const Glib::ustring &text,
               const time_t &date = time(nullptr));
  NoteListItem(const Note &note);

private:
  void set_widgets();
  void set_time(const time_t &date);
};

/*
** Widget that contains list of notes.
*/
class NotesListBox : public Gtk::ScrolledWindow {
  Gtk::ListBox notes_list;

public:
  NotesListBox();
  void add_item(NoteListItem &item);
};

/*
** Editable multiline text field with Markdown
** syntax highlighting
**
** @css-class: note-edit
*/
class NoteEdit : public Gtk::ScrolledWindow {
  Gtk::TextView text_view;
  MarkdownParser parser;

public:
  NoteEdit();

private:
  enum Lexs {
    AddImage,
  };
  void format_buffer();
  void check_lexs();
  void handle_new_item();
};

#endif // WIDGETS_H_SENTRY
