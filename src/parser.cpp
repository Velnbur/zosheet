#include "parser.h"
#include "glibmm/ustring.h"
#include "gtkmm/headerbar.h"
#include "gtkmm/textchildanchor.h"
#include "gtkmm/textiter.h"
#include "gtkmm/widget.h"
#include <iostream>
#include <ostream>

enum {
  HEADER1_TEXT_SIZE = 19,
  HEADER2_TEXT_SIZE = 17,
  HEADER3_TEXT_SIZE = 15,
  MAX_HEADER_LEVEL = 3,
  MAX_SYMBOL_LENGTH = 3, // number of '*' or '_' symbols that must be counted
                         // standing after each other, it is also number of
                         // elements minus 1 in 'enum TextStyle'.
};

Parser::Lexema::Lexema(uint start, uint end, const Glib::ustring &tag_name)
    : start(start), end(end), tag_name(tag_name)
{
}

MarkdownParser::MarkdownParser() : lexs() {}

MarkdownParser::~MarkdownParser()
{
  if (!lexs.size())
    return;

  for (auto tmp : lexs)
    delete tmp;
}

void MarkdownParser::restart()
{
  for (auto tmp : lexs)
    delete tmp;
  lexs.clear();
}

static const Glib::ustring BOLD_TAG = "bold";
static const Glib::ustring BLOCK_TAG = "block";
static const Glib::ustring ITALIC_TAG = "italic";
static const Glib::ustring HEADER1_TAG = "header1";
static const Glib::ustring HEADER2_TAG = "header2";
static const Glib::ustring HEADER3_TAG = "header3";
static const Glib::ustring LIST_ITEM_TAG = "list-item";
static const Glib::ustring BACKQUOTES_TAG = "backquotes";

/*
** Add new tags, that are gonna be needed further, to TagTable
** of existing TextBuffer.
**
** Tags:
**   -- 'bold' - used for bold text surrounded by "__" or "**"
**   -- 'italic' - for italic text surrounded by "_" or "*"
**   -- tags for header with three levels:
**        (i) 'header1' - biggest header with HEADER1_TEXT_SIZE. Starts with '#'
**       (ii) 'header2' - header with HEADER2_TEXT_SIZE. Starts with '##'
**      (iii) 'header3' - header with HEADER3_TEXT_SIZE. Starts with '###'
*/
void MarkdownParser::set_tags(Glib::RefPtr<Gtk::TextBuffer::TagTable> tag_table)
{
  // Add HEADER1 tag
  auto header1_tag = Gtk::TextBuffer::Tag::create(HEADER1_TAG);
  header1_tag->property_font() =
      "Jetbrains Mono 19"; // TODO: there must another
                           // way for resizing fonts
  header1_tag->property_weight() = Pango::Weight::BOLD;
  tag_table->add(header1_tag);

  // Add HEADER2 tag
  auto header2_tag = Gtk::TextBuffer::Tag::create(HEADER2_TAG);
  header2_tag->property_font() =
      "Jetbrains Mono 17"; // TODO: there must another
                           // way for resizing fonts
  header2_tag->property_weight() = Pango::Weight::BOLD;
  tag_table->add(header2_tag);

  // Add HEADER3 tag
  auto header3_tag = Gtk::TextBuffer::Tag::create(HEADER3_TAG);
  header3_tag->property_font() =
      "Jetbrains Mono 15"; // TODO: there must another
                           // way for resizing fonts
  header3_tag->property_weight() = Pango::Weight::BOLD;
  tag_table->add(header3_tag);

  // Add italic tag
  auto italic_tag = Gtk::TextBuffer::Tag::create(ITALIC_TAG);
  italic_tag->property_style() = Pango::Style::ITALIC;
  tag_table->add(italic_tag);

  // Add bold tag
  auto bold_tag = Gtk::TextBuffer::Tag::create(BOLD_TAG);
  bold_tag->property_weight() = Pango::Weight::BOLD;
  tag_table->add(bold_tag);

  // Add block tag
  auto block_tag = Gtk::TextBuffer::Tag::create(BLOCK_TAG);
  block_tag->property_paragraph_background() = "#f9f9fa";
  block_tag->property_foreground() = "#a7a4a5";
  block_tag->property_indent() = 10;
  block_tag->property_pixels_above_lines() = 5;
  block_tag->property_pixels_below_lines() = 5;
  tag_table->add(block_tag);

  // Add list tag
  auto list_item_tag = Gtk::TextBuffer::Tag::create(LIST_ITEM_TAG);
  list_item_tag->property_indent() = 15;
  tag_table->add(list_item_tag);

  // Add backqoutes tag
  auto backquotes_tag = Gtk::TextBuffer::Tag::create(BACKQUOTES_TAG);
  backquotes_tag->property_background() = "#b8b8b8";
  backquotes_tag->property_foreground() = "black";
  tag_table->add(backquotes_tag);
}

MarkdownParser::BoldText::BoldText(uint start, uint end)
    : Lexema(start, end, BOLD_TAG)
{
}

MarkdownParser::BlockText::BlockText(uint start, uint end)
    : Lexema(start, end, BLOCK_TAG)
{
}

MarkdownParser::ListItemText::ListItemText(uint start, uint end)
    : Lexema(start, end, LIST_ITEM_TAG)
{
}

MarkdownParser::ItalicText::ItalicText(uint start, uint end)
    : Lexema(start, end, ITALIC_TAG)
{
}

MarkdownParser::BackquotesText::BackquotesText(uint start, uint end)
    : Lexema(start, end, BACKQUOTES_TAG)
{
}

/*
** Small help function for initializing HeaderText struct with
** different tag names depending on input enum value
*/
const Glib::ustring &
MarkdownParser::_get_header_tag_by_level(MarkdownParser::HeaderLevel lev)
{
  switch (lev) {
  case HeaderLevel::ONE:
    return HEADER1_TAG;
  case HeaderLevel::TWO:
    return HEADER2_TAG;
  case HeaderLevel::THREE:
    return HEADER3_TAG;
  }
  return HEADER1_TAG;
}

MarkdownParser::HeaderText::HeaderText(uint start, uint end,
                                       MarkdownParser::HeaderLevel lev)
    : Lexema(start, end, _get_header_tag_by_level(lev))
{
}

/*
** Define by first one or two chars what lexema it is.
** @return element forn LEXS enum.
*/
MarkdownParser::LEXS MarkdownParser::check_lexema(const Gtk::TextIter &start)
{
  auto buff = start.get_buffer();

  switch (*start) {
  case '-': {
    if (start.is_start())
      return LEXS::LIST_ITEM;
    else if (*(buff->get_iter_at_offset(start.get_offset() - 1)) == '\n')
      return LEXS::LIST_ITEM;
    break;
  }
  case '#': {
    if (start.is_start())
      return LEXS::HEADER;
    else if (*(buff->get_iter_at_offset(start.get_offset() - 1)) == '\n')
      return LEXS::HEADER;
    break;
  }
  case '>': {
    if (start.is_start())
      return LEXS::BLOCK;
    // if it is not start -> check previous char if it s a line feed
    else if (*(buff->get_iter_at_offset(start.get_offset() - 1)) == '\n')
      return LEXS::BLOCK;
    break;
  }
  case '_':
    return LEXS::ITALIC;
    break;
  case '*': {
    auto next = buff->get_iter_at_offset(start.get_offset() + 1);
    if (*next == '*')
      return LEXS::BOLD;
    else
      return LEXS::ITALIC;
    break;
  }
  case '`': {
    return LEXS::BACKQUOTES;
  }
  }

  return LEXS::PLAIN;
}

/*
** Define if text is surrounded by '**', @return true if it is.
** Also through @start and @end return positions of bold text
** and add BoldText struct to @lexs.
*/
bool MarkdownParser::parse_bold(Gtk::TextIter &start, Gtk::TextIter &end)
{
  auto tmp_start = start;
  if (*tmp_start != '*')
    throw "Wrong input to 'parse_bold' method. Something went totally wrong"; // TODO
                                                                              //
  ++tmp_start; // increment to skip first two '_' ot '*' symbols
  if (*tmp_start != '*')
    throw "Wrong input to 'parse_bold' method. Something went totally wrong"; // TODO
  ++tmp_start;

  auto last = *tmp_start;
  for (auto tmp = ++tmp_start; tmp != end; ++tmp) {
    if (*tmp == '*' && last == '*') {
      end = tmp;
      lexs.push_back(new BoldText(start.get_offset(), (++end).get_offset()));
      return true;
    }
    last = *tmp;
  }
  return false;
}

/*
** Parse current string until line feed as 'block' that starts with '>'.
*/
void MarkdownParser::parse_block(Gtk::TextIter &start, Gtk::TextIter &end)
{
  if (*start != '>')
    throw "Wrong input to 'parse_block' method. Something went totally wrong"; // TODO

  for (auto tmp = ++start; tmp != end; ++tmp) {
    if (*tmp == '\n') {
      end = tmp;
      break;
    }
  }

  lexs.push_back(new BlockText((--start).get_offset(), end.get_offset()));
  end = ++start;
  --start;
}

/*
** Parse current string until line feed as 'block' that starts with '>'.
*/
void MarkdownParser::parse_list_item(Gtk::TextIter &start, Gtk::TextIter &end)
{
  if (*start != '-')
    throw "Wrong input to 'parse_list_item' method. Something went totally "
          "wrong"; // TODO

  for (auto tmp = ++start; tmp != end; ++tmp) {
    if (*tmp == '\n') {
      end = tmp;
      break;
    }
  }

  lexs.push_back(new ListItemText((--start).get_offset(), end.get_offset()));
}

/*
** Define if text is surrounded by '_' or '*', @return true if it is.
** Also through @start and @end return positions of italic text
** and add ItalicText struct to @lexs.
*/
bool MarkdownParser::parse_italic(Gtk::TextIter &start, Gtk::TextIter &end)
{
  // Must check if first was star -> it must be closed with a star
  if (*start != '_' && *start != '*')
    throw "Wrong input to 'parse_italic' method. Something Went totally wrong"; // TODO
  auto symbol = *start;
  auto tmp_start = start;

  for (auto tmp = ++tmp_start; tmp != end; ++tmp) {
    if (*tmp == symbol) {
      end = tmp;
      lexs.push_back(new ItalicText(start.get_offset(), (++end).get_offset()));
      return true;
    }
  }
  return false;
}

/*
** Define if text is surrounded '`', @return true if it is.
** Also through @start and @end return positions of highlighted text
** and add BackquotesText struct to @lexs.
*/
bool MarkdownParser::parse_backquotes(Gtk::TextIter &start, Gtk::TextIter &end)
{
  // Must check if first was star -> it must be closed with a star
  if (*start != '`')
    throw "Wrong input to 'parse_backquotes' method. Something Went totally "
          "wrong"; // TODO
  auto tmp_start = start;

  for (auto tmp = ++tmp_start; tmp != end; ++tmp) {
    if (*tmp == '`') {
      end = tmp;
      lexs.push_back(
          new BackquotesText(start.get_offset(), (++end).get_offset()));
      return true;
    }
  }
  return false;
}

/*
** Parse current string until line feed as 'header' that starts with '#'.
** Depending on number of '#' at start of the line -> add HeaderText to
** @lexs with different HeaderLevel.
*/
void MarkdownParser::parse_header(Gtk::TextIter &start, Gtk::TextIter &end)
{
  if (*start != '#')
    throw "Wrong input ot 'parse_header' method. Something went totally wrong"; // TODO

  uint level = HeaderLevel::ONE;
  auto last = *start;
  bool flag = true; // flag that '#' goes after each other at start

  for (auto tmp = ++start; tmp != end; ++tmp) {
    if (*tmp == '\n') {
      end = tmp;
      break;
    } else if (flag && *tmp == '#' && last == '#') {
      if (level < MAX_HEADER_LEVEL)
        ++level;
    } else { // stop incrementing header level
      flag = false;
    }
    last = *tmp;
  }

  lexs.push_back(new HeaderText((--start).get_offset(), end.get_offset(),
                                static_cast<HeaderLevel>(level)));
}

void MarkdownParser::parse_buffer(Glib::RefPtr<Gtk::TextBuffer> buff)
{
  Gtk::TextIter start, end;
  buff->get_bounds(start, end);

  for (auto tmp = start; tmp != end; ++tmp) {
    auto tmp_end = end;
    switch (check_lexema(tmp)) {
    case LEXS::PLAIN: {
      break;
    }
    case LEXS::HEADER: {
      parse_header(tmp, tmp_end);
      tmp = tmp_end;
      break;
    }
    case LEXS::BLOCK: {
      parse_block(tmp, tmp_end);
      tmp = tmp_end;
      break;
    }
    case LEXS::LIST_ITEM: {
      parse_list_item(tmp, tmp_end);
      tmp = tmp_end;
      break;
    }
    case LEXS::ITALIC: {
      if (parse_italic(tmp, tmp_end))
        tmp = tmp_end;
      break;
    }
    case LEXS::BOLD: {
      if (parse_bold(tmp, tmp_end))
        tmp = tmp_end;
      break;
    }
    case LEXS::BACKQUOTES: {
      if (parse_backquotes(tmp, tmp_end))
        tmp = tmp_end;
      break;
    }
    }
  }

  use_tags(buff);
}

void MarkdownParser::use_tags(Glib::RefPtr<Gtk::TextBuffer> buff)
{
  Gtk::TextIter start, end;
  buff->get_bounds(start, end);
  buff->remove_all_tags(start, end);

  size_t len = lexs.size();
  for (uint i = 0; i < len; ++i) {
    start = buff->get_iter_at_offset(lexs[i]->start);
    end = buff->get_iter_at_offset(lexs[i]->end);
    buff->apply_tag_by_name(lexs[i]->tag_name, start, end);
  }
}
