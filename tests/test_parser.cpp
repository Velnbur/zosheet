#include "../src/parser.cpp"
#include "ctest.h"
#include "glibmm/refptr.h"
#include "gtkmm/application.h"
#include "gtkmm/textbuffer.h"
#include "gtkmm/textiter.h"
#include "parser.h"
#include <iostream>

class TestMarkdownParser : public MarkdownParser {
public:
  using MarkdownParser::check_lexema;
  using MarkdownParser::lexs;
  using MarkdownParser::LEXS;
  using MarkdownParser::parse_block;
  using MarkdownParser::parse_bold;
  using MarkdownParser::parse_header;
  using MarkdownParser::parse_italic;
};

CTEST_DATA(parser)
{
  TestMarkdownParser parser;
  Glib::RefPtr<Gtk::Application> app;
  Glib::RefPtr<Gtk::TextBuffer> buff;
};

CTEST_SETUP(parser)
{
  data->app = Gtk::Application::create("zosheet.test");
  data->buff = Gtk::TextBuffer::create();
  data->parser.set_tags(data->buff->get_tag_table());
}

CTEST2(parser, parse_check_lexema)
{
  Gtk::TextIter start, end;
  // check header
  data->buff->set_text("# Header");
  data->buff->get_bounds(start, end);

  ASSERT_EQUAL(TestMarkdownParser::LEXS::HEADER,
               data->parser.check_lexema(start));

  // check bold
  data->buff->set_text("**bold**");
  data->buff->get_bounds(start, end);

  ASSERT_EQUAL(TestMarkdownParser::LEXS::BOLD,
               data->parser.check_lexema(start));

  // check italic
  data->buff->set_text("_italic");
  data->buff->get_bounds(start, end);

  ASSERT_EQUAL(TestMarkdownParser::LEXS::ITALIC,
               data->parser.check_lexema(start));

  // check italic
  data->buff->set_text("*italic*");
  data->buff->get_bounds(start, end);

  ASSERT_EQUAL(TestMarkdownParser::LEXS::ITALIC,
               data->parser.check_lexema(start));

  // check header
  data->buff->set_text("> block");
  data->buff->get_bounds(start, end);

  ASSERT_EQUAL(TestMarkdownParser::LEXS::BLOCK,
               data->parser.check_lexema(start));

  // check plain
  data->buff->set_text("plain");
  data->buff->get_bounds(start, end);

  ASSERT_EQUAL(TestMarkdownParser::LEXS::PLAIN,
               data->parser.check_lexema(start));
}

CTEST2(parser, parse_header_one_line)
{
  Gtk::TextIter start, end;
  data->buff->set_text("# Header");
  data->buff->get_bounds(start, end);

  try {
    data->parser.parse_header(start, end);
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), HEADER1_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(8, data->parser.lexs[0]->end);

  data->parser.lexs.clear();
}

CTEST2(parser, parse_header_two_lines)
{
  Gtk::TextIter start, end;
  data->buff->set_text("# Header\nhello");
  data->buff->get_bounds(start, end);

  try {
    data->parser.parse_header(start, end);
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), HEADER1_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(8, data->parser.lexs[0]->end);

  data->parser.lexs.clear();
}

CTEST2(parser, parse_header_level2)
{
  Gtk::TextIter start, end;
  data->buff->set_text("## Header");
  data->buff->get_bounds(start, end);

  try {
    data->parser.parse_header(start, end);
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), HEADER2_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(9, data->parser.lexs[0]->end);
  data->parser.lexs.clear();
}

CTEST2(parser, parse_header_level3)
{
  Gtk::TextIter start, end;
  data->buff->set_text("### Header hello\n hello");
  data->buff->get_bounds(start, end);

  try {
    data->parser.parse_header(start, end);
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), HEADER3_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(16, data->parser.lexs[0]->end);
  data->parser.lexs.clear();
}

CTEST2(parser, parse_italic)
{
  Gtk::TextIter start, end;
  data->buff->set_text("_italic_");
  data->buff->get_bounds(start, end);

  try {
    ASSERT_TRUE(data->parser.parse_italic(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), ITALIC_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(8, data->parser.lexs[0]->end);

  data->parser.lexs.clear();
}

CTEST2(parser, parse_italic_stars)
{
  Gtk::TextIter start, end;
  data->buff->set_text("*italic*");
  data->buff->get_bounds(start, end);

  try {
    ASSERT_TRUE(data->parser.parse_italic(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), ITALIC_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(8, data->parser.lexs[0]->end);

  data->parser.lexs.clear();
}

CTEST2(parser, parse_italic_excp)
{
  Gtk::TextIter start, end;
  data->buff->set_text("*italic");
  data->buff->get_bounds(start, end);

  try {
    ASSERT_FALSE(data->parser.parse_italic(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
  data->parser.lexs.clear();
}

CTEST2(parser, parse_italic_multiple)
{
  Gtk::TextIter start, end;
  data->buff->set_text("*italic* and _italic_");
  data->buff->get_bounds(start, end);
  auto tmp_end = end;

  try {
    ASSERT_EQUAL(true, data->parser.parse_italic(start, tmp_end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), ITALIC_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(8, data->parser.lexs[0]->end);

  start = data->buff->get_iter_at_offset(tmp_end.get_offset() + 5);
  try {
    ASSERT_TRUE(data->parser.parse_italic(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[1]->tag_name.c_str(), ITALIC_TAG.c_str());
  ASSERT_EQUAL(13, data->parser.lexs[1]->start);
  ASSERT_EQUAL(21, data->parser.lexs[1]->end);

  data->parser.lexs.clear();
}

CTEST2(parser, parse_bold)
{
  Gtk::TextIter start, end;
  data->buff->set_text("**bold**");
  data->buff->get_bounds(start, end);

  try {
    ASSERT_TRUE(data->parser.parse_bold(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), BOLD_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(8, data->parser.lexs[0]->end);
  data->parser.lexs.clear();
}

CTEST2(parser, parse_bold_excp)
{
  Gtk::TextIter start, end;
  data->buff->set_text("**bold*");
  data->buff->get_bounds(start, end);

  try {
    ASSERT_FALSE(data->parser.parse_bold(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  data->buff->set_text("**bold_");
  data->buff->get_bounds(start, end);
  try {
    ASSERT_FALSE(data->parser.parse_bold(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  data->buff->set_text("**bold__");
  data->buff->get_bounds(start, end);
  try {
    ASSERT_FALSE(data->parser.parse_bold(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
}

CTEST2(parser, parse_bold_multiple)
{
  Gtk::TextIter start, end;
  data->buff->set_text("**bold** and **bold**");
  data->buff->get_bounds(start, end);
  auto tmp_end = end;

  try {
    ASSERT_TRUE(data->parser.parse_bold(start, tmp_end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), BOLD_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(8, data->parser.lexs[0]->end);

  start = data->buff->get_iter_at_offset(tmp_end.get_offset() + 5);
  try {
    ASSERT_TRUE(data->parser.parse_bold(start, end));
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[1]->tag_name.c_str(), BOLD_TAG.c_str());
  ASSERT_EQUAL(13, data->parser.lexs[1]->start);

  ASSERT_EQUAL(21, data->parser.lexs[1]->end);

  data->parser.lexs.clear();
}

CTEST2(parser, parse_block)
{
  Gtk::TextIter start, end;
  data->buff->set_text("> block");
  data->buff->get_bounds(start, end);

  try {
    data->parser.parse_block(start, end);
  } catch (char const *err) {
    std::cout << err << std::endl;
  }

  ASSERT_STR(data->parser.lexs[0]->tag_name.c_str(), BLOCK_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[0]->start);
  ASSERT_EQUAL(7, data->parser.lexs[0]->end);

  data->buff->set_text("> block\n> hello");
  data->buff->get_bounds(start, end);

  auto tmp_end = end;

  try {
    data->parser.parse_block(start, tmp_end);
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
  ASSERT_STR(data->parser.lexs[1]->tag_name.c_str(), BLOCK_TAG.c_str());
  ASSERT_EQUAL(0, data->parser.lexs[1]->start);
  ASSERT_EQUAL(7, data->parser.lexs[1]->end);

  start = data->buff->get_iter_at_offset(tmp_end.get_offset() + 1);
  try {
    data->parser.parse_block(start, end);
  } catch (char const *err) {
    std::cout << err << std::endl;
  }
  ASSERT_STR(data->parser.lexs[2]->tag_name.c_str(), BLOCK_TAG.c_str());
  ASSERT_EQUAL(8, data->parser.lexs[2]->start);
  ASSERT_EQUAL(15, data->parser.lexs[2]->end);
}
