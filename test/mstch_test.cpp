#include <iostream>
#include <fstream>
#include "string"

#include <gtest/gtest.h>
#include "mstch/mstch.hpp"
#include "test/mstch_test_data.hpp"


std::string load_file(std::string file_name) {
  std::string file_path = file_name;
  #ifdef _WIN32
  std::replace(file_path.begin(), file_path.end(), '/', '\\');
  #endif
  const std::ifstream input_stream(file_path, std::ifstream::binary);
  if (input_stream.fail()) {
      throw std::runtime_error("Failed to open file");
  }
  std::stringstream buffer;
  buffer << input_stream.rdbuf();
  return buffer.str();
}

#define MSTCH_TEST(x) TEST(MstchTests, x) { \
  const std::string expected = load_file("test/data/" #x ".txt"); \
  const std::string tmpl = load_file("test/data/" #x ".mustache"); \
  EXPECT_EQ(expected, mstch::render(tmpl, x ## _data)); \
}

#define MSTCH_PARTIAL_TEST(x) TEST(MstchTests, x) { \
  const std::string expected = load_file("test/data/" #x ".txt"); \
  const std::string tmpl = load_file("test/data/" #x ".mustache"); \
  const std::string partial = load_file("test/data/" #x ".partial"); \
  EXPECT_EQ(expected, mstch::render(tmpl, x ## _data, {{"partial", partial}})); \
}

MSTCH_TEST(ampersand_escape)
MSTCH_TEST(apostrophe)
MSTCH_TEST(array_of_strings)
MSTCH_TEST(backslashes)
MSTCH_TEST(bug_11_eating_whitespace)
MSTCH_TEST(bug_length_property)
MSTCH_TEST(changing_delimiters)
MSTCH_TEST(comments)
MSTCH_TEST(complex)
MSTCH_TEST(context_lookup)
MSTCH_TEST(delimiters)
MSTCH_TEST(disappearing_whitespace)
MSTCH_TEST(dot_notation)
MSTCH_TEST(double_render)
MSTCH_TEST(empty_list)
MSTCH_TEST(empty_sections)
MSTCH_TEST(empty_string)
MSTCH_TEST(empty_template)
MSTCH_TEST(error_eof_in_section)
MSTCH_TEST(error_eof_in_tag)
MSTCH_TEST(error_not_found)
MSTCH_TEST(escaped)
MSTCH_TEST(falsy)
MSTCH_TEST(falsy_array)
MSTCH_TEST(grandparent_context)
MSTCH_TEST(higher_order_sections)
MSTCH_TEST(implicit_iterator)
MSTCH_TEST(included_tag)
MSTCH_TEST(inverted_section)
MSTCH_TEST(keys_with_questionmarks)
MSTCH_TEST(multiline_comment)
MSTCH_TEST(nested_dot)
MSTCH_TEST(nested_higher_order_sections)
MSTCH_TEST(nested_iterating)
MSTCH_TEST(nesting)
MSTCH_TEST(nesting_same_name)
MSTCH_TEST(null_lookup_array)
MSTCH_TEST(null_lookup_object)
MSTCH_TEST(null_string)
MSTCH_TEST(null_view)
MSTCH_PARTIAL_TEST(partial_array)
MSTCH_PARTIAL_TEST(partial_array_of_partials)
MSTCH_PARTIAL_TEST(partial_array_of_partials_implicit)
MSTCH_PARTIAL_TEST(partial_empty)
MSTCH_PARTIAL_TEST(partial_template)
MSTCH_PARTIAL_TEST(partial_view)
MSTCH_PARTIAL_TEST(partial_whitespace)
MSTCH_TEST(recursion_with_same_names)
MSTCH_TEST(reuse_of_enumerables)
MSTCH_TEST(section_as_context)
MSTCH_PARTIAL_TEST(section_functions_in_partials)
MSTCH_TEST(simple)
MSTCH_TEST(string_as_context)
MSTCH_TEST(two_in_a_row)
MSTCH_TEST(two_sections)
MSTCH_TEST(unescaped)
MSTCH_TEST(whitespace)
MSTCH_TEST(zero_view)
