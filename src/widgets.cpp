#include "widgets.h"
#include "gtkmm/enums.h"
#include "gtkmm/headerbar.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/textbuffer.h"
#include "gtkmm/textiter.h"
#include "gtkmm/texttagtable.h"
#include "gtkmm/textview.h"
#include "objects.h"
#include "pangomm/fontdescription.h"
#include "sigc++/functors/mem_fun.h"
#include "sigc++/functors/ptr_fun.h"
#include <ctime>
#include <iostream>
#include <sys/types.h>

NoteListItem::NoteListItem(const Note &note)
    : title(note.name), text(note.text), h_box(Gtk::Orientation::HORIZONTAL),
      v_box(Gtk::Orientation::VERTICAL)
{
  this->set_time(note.date);
  this->set_widgets();
}

NoteListItem::NoteListItem(const Glib::ustring &title,
                           const Glib::ustring &text, const time_t &date)
    : title(title), text(text), h_box(Gtk::Orientation::HORIZONTAL),
      v_box(Gtk::Orientation::VERTICAL)
{
  this->set_time(date);
  this->set_widgets();
}

#define DATE_MAX_SIZE 20
void NoteListItem::set_time(const time_t &date)
{
  char buff[DATE_MAX_SIZE];
  strftime(buff, DATE_MAX_SIZE, "%x",
           localtime(&date)); // format date to "day.month.year" look
                              // TODO: may be there is another, better way
                              // of formatting time in C++
  this->date.set_text(buff);
}

/*
** Initialize child widgets, set their parametrs and position
*/
void NoteListItem::set_widgets()
{
  h_box.append(title);
  title.set_halign(Gtk::Align::START);
  title.set_hexpand(true);
  title.set_wrap(true); // Make text hide behind other widgets
  title.set_ellipsize(Pango::EllipsizeMode::END); // if it is too long

  title.set_name("note-item-title"); // set CSS class for titles to make text
                                     // bigger and bold in 'styles/styles.css'
  h_box.append(date);
  date.set_halign(Gtk::Align::END);
  v_box.append(h_box);
  v_box.append(text);
  text.set_xalign(0); // Align text straigt to the left

  this->set_name("note-item");
  this->append(v_box);
}

NotesListBox::NotesListBox() : notes_list()
{
  this->set_child(notes_list);
  this->set_vexpand(true);
}

/*
** Add item to list.
** Gives odd and even items different CSS classes
** for styling in 'styles/styles.css'
*/
void NotesListBox::add_item(NoteListItem &item)
{
  static bool flag = true; // 'static' mod used for knowing last added item
                           // to make next with different CSS class
  if (flag) {
    item.set_name("note-item-1");
  } else {
    item.set_name("note-item-2");
  }
  flag = !flag;
  notes_list.append(item);
}

NoteEdit::NoteEdit() : Gtk::ScrolledWindow(), parser()
{
  set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
  set_hexpand(true);

  text_view.set_name("note-edit");
  text_view.set_margin(5);
  auto buff = text_view.get_buffer();
  parser.set_tags(buff->get_tag_table());

  buff->signal_changed().connect(
      sigc::mem_fun(*this, &NoteEdit::format_buffer));
  set_child(text_view);
}

/*
** callback function for 'buffer_changed' signal
*/
void NoteEdit::format_buffer()
{
  parser.restart();
  auto buff = text_view.get_buffer();
  parser.parse_buffer(buff);
}
