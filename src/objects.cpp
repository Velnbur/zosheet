#include "objects.h"
#include "glibmm/ustring.h"

Note::Note(const Glib::ustring &name, const Glib::ustring &text,
           const time_t &date)
    : name(name), text(text), date(date) {}
