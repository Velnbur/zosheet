#ifndef PARSER_H_SENTRY
#define PARSER_H_SENTRY

#include "glibmm/refptr.h"
#include "glibmm/ustring.h"
#include "gtkmm/textbuffer.h"
#include "gtkmm/textiter.h"
#include "gtkmm/textview.h"
#include <vector>

/*
** Abstract class for all parsers.
** @T - must be iterable buffer
*/
class Parser {
public:
  virtual void set_tags(Glib::RefPtr<Gtk::TextBuffer::TagTable> table) = 0;
  virtual void parse_buffer(Glib::RefPtr<Gtk::TextBuffer> buff) = 0;
  virtual void restart() = 0;

protected:
  struct Lexema {
    uint start;
    uint end;
    const Glib::ustring &tag_name;
    Lexema(uint start, uint end, const Glib::ustring &tag_name);
  };
};

/*
** Class for parsing markdown
*/
class MarkdownParser : public Parser {
public:
  MarkdownParser();
  ~MarkdownParser();
  virtual void set_tags(Glib::RefPtr<Gtk::TextBuffer::TagTable> table);
  virtual void parse_buffer(Glib::RefPtr<Gtk::TextBuffer> buff);
  virtual void restart();

protected:
  struct BoldText : public Lexema {
    BoldText(uint start, uint end);
  };

  struct ItalicText : public Lexema {
    ItalicText(uint start, uint end);
  };

  enum HeaderLevel {
    ONE = 1,
    TWO,
    THREE,
  };

  enum LEXS { HEADER, BLOCK, ITALIC, BOLD, PLAIN, LIST_ITEM, BACKQUOTES };

  struct HeaderText : public Lexema {
    HeaderText(uint start, uint end, HeaderLevel lev);
  };

  struct BlockText : public Lexema {
    BlockText(uint start, uint end);
  };

  struct ListItemText : public Lexema {
    ListItemText(uint start, uint end);
  };

  struct BackquotesText : public Lexema {
    BackquotesText(uint start, uint end);
  };

  std::vector<Lexema *> lexs;

  bool parse_bold(Gtk::TextIter &start, Gtk::TextIter &end);
  bool parse_italic(Gtk::TextIter &start, Gtk::TextIter &end);
  void parse_header(Gtk::TextIter &start, Gtk::TextIter &end);
  void parse_block(Gtk::TextIter &start, Gtk::TextIter &end);
  void parse_list_item(Gtk::TextIter &start, Gtk::TextIter &end);
  bool parse_backquotes(Gtk::TextIter &start, Gtk::TextIter &end);
  void use_tags(Glib::RefPtr<Gtk::TextBuffer> buff);

  static const Glib::ustring &
  _get_header_tag_by_level(MarkdownParser::HeaderLevel lev);
  static LEXS check_lexema(const Gtk::TextIter &start);
};

#endif // PARSER_H_SENTRY
