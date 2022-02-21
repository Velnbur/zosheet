#ifndef OBJECTS_H_SENTRY
#define OBJECTS_H_SENTRY

#include "glibmm/ustring.h"
#include <ctime>

struct Note {
  Glib::ustring name;
  Glib::ustring text;
  time_t date;

  Note(const Glib::ustring &name, const Glib::ustring &text,
       const time_t &date);
};

#endif // OBJECTS_H_SENTRY
