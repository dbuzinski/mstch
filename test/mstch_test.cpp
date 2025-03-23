#include "mstch/mstch.hpp"
#include <gtest/gtest.h>

TEST(RenderTest, AmpersandEscape) {
  std::string input = "{{&message}}\n";
  mstch::node data = mstch::map{{"message", std::string{"Some <code>"}}};

  std::string expected = "Some <code>\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Apostrophe) {
  std::string input = "{{apos}}{{control}}\n";
  mstch::node data =
      mstch::map{{"apos", std::string{"'"}}, {"control", std::string{"X"}}};

  std::string expected = "&#39;X\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ArrayOfStrings) {
  std::string input = "{{#array_of_strings}}{{.}} {{/array_of_strings}}\n";
  mstch::node data =
      mstch::map{{"array_of_strings",
                  mstch::array{std::string{"hello"}, std::string{"world"}}}};

  std::string expected = "hello world \n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Backslashes) {
  const std::string input{"* {{value}}\n"
                          "* {{{value}}}\n"
                          "* {{&value}}\n"
                          "<script>\n"
                          "foo = { bar: 'abc\\\"xyz\\\"' };\n"
                          "foo = { bar: 'x\\'y' };\n"
                          "</script>\n"};
  mstch::node data = mstch::map{{"value", std::string{"\\abc"}}};

  const std::string expected{"* \\abc\n"
                             "* \\abc\n"
                             "* \\abc\n"
                             "<script>\n"
                             "foo = { bar: 'abc\\\"xyz\\\"' };\n"
                             "foo = { bar: 'x\\'y' };\n"
                             "</script>\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Bug11EatingWhitespace) {
  std::string input = "{{tag}} foo\n";
  mstch::node data = mstch::map{{"tag", std::string{"yo"}}};

  std::string expected = "yo foo\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, BugLengthProperty) {
  std::string input =
      "{{#length}}The length variable is: {{length}}{{/length}}\n";
  mstch::node data = mstch::map{{"length", std::string{"hello"}}};

  std::string expected = "The length variable is: hello\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ChangingDelimiters) {
  std::string input = "{{=<% %>=}}<% foo %> {{foo}} <%{bar}%> {{{bar}}}\n";
  mstch::node data = mstch::map{{"foo", std::string{"foooooooooooooo"}},
                                {"bar", std::string{"<b>bar!</b>"}}};

  std::string expected = "foooooooooooooo {{foo}} <b>bar!</b> {{{bar}}}\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Comments) {
  std::string input =
      "<h1>{{title}}{{! just something interesting... or not... }}</h1>\n";
  mstch::node data = mstch::map{{"title", mstch::lambda{[]() -> mstch::node {
                                   return std::string{"A Comedy of Errors"};
                                 }}}};

  std::string expected = "<h1>A Comedy of Errors</h1>\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ContextLookup) {
  std::string input = "{{#outer}}{{#second}}{{id}}{{/second}}{{/outer}}\n";
  mstch::map data = mstch::map{
      {"outer", mstch::map{{"id", 1}, {"second", mstch::map{{"nothing", 2}}}}}};

  std::string expected = "1\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Delimiters) {
  std::string input{"{{=<% %>=}}*\n"
                    "<% first %>\n"
                    "* <% second %>\n"
                    "<%=| |=%>\n"
                    "* | third |\n"
                    "|={{ }}=|\n"
                    "* {{ fourth }}\n"};
  mstch::map data = mstch::map{
      {"first", std::string{"It worked the first time."}},
      {"second", std::string{"And it worked the second time."}},
      {"third", std::string{"Then, surprisingly, it worked the third time."}},
      {"fourth", std::string{"Fourth time also fine!."}}};

  std::string expected{"*\n"
                       "It worked the first time.\n"
                       "* And it worked the second time.\n"
                       "* Then, surprisingly, it worked the third time.\n"
                       "* Fourth time also fine!.\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, DisappearingWhitespace) {
  std::string input = "{{#bedrooms}}{{total}}{{/bedrooms}} BED\n";
  mstch::map data = mstch::map{{"bedrooms", true}, {"total", 1}};

  std::string expected = "1 BED\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, DoubleRender) {
  std::string input = "{{#foo}}{{bar}}{{/foo}}\n";
  mstch::node data = mstch::map{{"foo", true},
                                {"bar", std::string{"{{win}}"}},
                                {"win", std::string{"FAIL"}}};

  std::string expected = "{{win}}\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, EmptyList) {
  std::string input{"These are the jobs:\n"
                    "{{#jobs}}\n"
                    "{{.}}\n"
                    "{{/jobs}}\n"};
  mstch::node data = mstch::map{{"jobs", mstch::array{}}};

  std::string expected = "These are the jobs:\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, EmptySections) {
  std::string input = "{{#foo}}{{/foo}}foo{{#bar}}{{/bar}}\n";
  mstch::node data = mstch::map{};

  std::string expected = "foo\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, EmptyString) {
  std::string input = "{{description}}{{#child}}{{description}}{{/child}}\n";
  mstch::node data =
      mstch::map{{"description", std::string{"That is all!"}},
                 {"child", mstch::map{{"description", std::string{""}}}}};

  std::string expected = "That is all!\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, EmptyTemplate) {
  std::string input = "<html><head></head><body><h1>Test</h1></body></html>";
  mstch::node data = mstch::map{};

  std::string expected = "<html><head></head><body><h1>Test</h1></body></html>";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ErrorEofInSection) {
  std::string input = "yay{{#hello}}{{.}}";
  mstch::node data =
      mstch::map{{"hello", mstch::array{std::string{"a"}, std::string{"b"}}}};

  std::string expected = "yay";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ErrorEofInTag) {
  std::string input = "{{hello{{hello}}{{hello";
  mstch::node data = mstch::map{{"hello", std::string{"world"}}};

  std::string expected = "{{hello";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ErrorNotFound) {
  std::string input = "{{foo}}";
  mstch::node data = mstch::map{{"bar", 2}};

  std::string expected{};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Escaped) {
  std::string input{"<h1>{{title}}</h1>\n"
                    "And even {{entities}}, but not {{{entities}}}.\n"};
  mstch::node data = mstch::map{{"title", mstch::lambda{[]() -> mstch::node {
                                   return std::string{"Bear > Shark"};
                                 }}},
                                {"entities", mstch::lambda{[]() -> mstch::node {
                                   return std::string{"&quot; \"'<>/"};
                                 }}}};

  std::string expected{"<h1>Bear &gt; Shark</h1>\n"
                       "And even &amp;quot; &quot;&#39;&lt;&gt;&#x2F;, but not "
                       "&quot; \"'<>/.\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Falsy) {
  std::string input{"{{#emptyString}}empty string{{/emptyString}}\n"
                    "{{^emptyString}}inverted empty string{{/emptyString}}\n"
                    "{{#emptyArray}}empty array{{/emptyArray}}\n"
                    "{{^emptyArray}}inverted empty array{{/emptyArray}}\n"
                    "{{#zero}}zero{{/zero}}\n"
                    "{{^zero}}inverted zero{{/zero}}\n"
                    "{{#null}}null{{/null}}\n"
                    "{{^null}}inverted null{{/null}}\n"};
  mstch::node data = mstch::map{{"emptyString", std::string{""}},
                                {"emptyArray", mstch::array{}},
                                {"zero", 0},
                                {"null", mstch::node{}}};

  std::string expected{"\n"
                       "inverted empty string\n"
                       "\n"
                       "inverted empty array\n"
                       "\n"
                       "inverted zero\n"
                       "\n"
                       "inverted null\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, FalsyArray) {
  std::string input{"{{#list}}\n"
                    "{{#.}}{{#.}}{{.}}{{/.}}{{^.}}inverted {{/.}}{{/.}}\n"
                    "{{/list}}"};
  mstch::node data = mstch::map{
      {"list",
       mstch::array{mstch::array{std::string{""}, std::string{"emptyString"}},
                    mstch::array{mstch::array{}, std::string{"emptyArray"}},
                    mstch::array{0, std::string{"zero"}},
                    mstch::array{mstch::node{}, std::string{"null"}}}}};

  std::string expected{"inverted emptyString\n"
                       "inverted emptyArray\n"
                       "inverted zero\n"
                       "inverted null\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, GrandparentContext) {
  std::string input{"{{grand_parent_id}}\n"
                    "{{#parent_contexts}}\n"
                    "{{grand_parent_id}}\n"
                    "{{parent_id}}\n"
                    "{{#child_contexts}}\n"
                    "{{grand_parent_id}}\n"
                    "{{parent_id}}\n"
                    "{{child_id}}\n"
                    "{{/child_contexts}}\n"
                    "{{/parent_contexts}}\n"};
  mstch::node data = mstch::map{
      {"grand_parent_id", std::string{"grand_parent1"}},
      {"parent_contexts",
       mstch::array{
           mstch::map{
               {"parent_id", std::string{"parent1"}},
               {"child_contexts",
                mstch::array{
                    mstch::map{{"child_id", std::string{"parent1-child1"}}},
                    mstch::map{{"child_id", std::string{"parent1-child2"}}}}}},
           mstch::map{
               {"parent_id", std::string{"parent2"}},
               {"child_contexts",
                mstch::array{
                    mstch::map{{"child_id", std::string{"parent2-child1"}}},
                    mstch::map{
                        {"child_id", std::string{"parent2-child2"}}}}}}}}};

  std::string expected{"grand_parent1\n"
                       "grand_parent1\n"
                       "parent1\n"
                       "grand_parent1\n"
                       "parent1\n"
                       "parent1-child1\n"
                       "grand_parent1\n"
                       "parent1\n"
                       "parent1-child2\n"
                       "grand_parent1\n"
                       "parent2\n"
                       "grand_parent1\n"
                       "parent2\n"
                       "parent2-child1\n"
                       "grand_parent1\n"
                       "parent2\n"
                       "parent2-child2\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ImplicitIterator) {
  std::string input{"{{# data.author.twitter_id }}\n"
                    "<meta name=\"twitter:site:id\" content=\"{{.}}\">\n"
                    "{{/ data.author.twitter_id }}\n"
                    "\n"
                    "{{# data.author.name }}\n"
                    "<meta name=\"twitter:site\" content=\"{{.}}\">\n"
                    "{{/ data.author.name }}\n"};
  mstch::node data = mstch::map{
      {"data",
       mstch::map{{"author", mstch::map{{"twitter_id", 819606},
                                        {"name", std::string{"janl"}}}}}}};

  std::string expected{"<meta name=\"twitter:site:id\" content=\"819606\">\n"
                       "\n"
                       "<meta name=\"twitter:site\" content=\"janl\">\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, IncludedTag) {
  std::string input = "You said \"{{{html}}}\" today\n";
  mstch::node data = mstch::map{{"html", std::string{"I like {{mustache}}"}}};

  std::string expected = "You said \"I like {{mustache}}\" today\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, InvertedSection) {
  std::string input{"{{#repos}}<b>{{name}}</b>{{/repos}}\n"
                    "{{^repos}}No repos :({{/repos}}\n"
                    "{{^nothin}}Hello!{{/nothin}}\n"};
  mstch::node data = mstch::map{{"repos", mstch::array{}}};

  std::string expected{"\n"
                       "No repos :(\n"
                       "Hello!\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, KeysWithQuestionmarks) {
  std::string input{"{{#person?}}\n"
                    "  Hi {{name}}!\n"
                    "{{/person?}}\n"};
  mstch::node data =
      mstch::map{{"person?", mstch::map{{"name", std::string{"Jon"}}}}};

  std::string expected = "  Hi Jon!\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, MultilineComment) {
  std::string input{"{{!\n"
                    "\n"
                    "This is a multi-line comment.\n"
                    "\n"
                    "}}\n"
                    "Hello world!\n"};
  mstch::node data = mstch::map{};

  std::string expected = "Hello world!\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NestedDot) {
  std::string input = "{{#name}}Hello {{.}}{{/name}}";
  mstch::node data = mstch::map{{"name", std::string{"Bruno"}}};

  std::string expected = "Hello Bruno";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NestedHigherOrderSections) {
  std::string input =
      "{{#bold}}{{#person}}My name is {{name}}!{{/person}}{{/bold}}\n";
  mstch::node data = mstch::map{
      {"bold", mstch::lambda{[](const std::string &text) -> mstch::node {
         return std::string{"<b>"} + text + std::string{"</b>"};
       }}},
      {"person", mstch::map{{"name", std::string{"Jonas"}}}}};

  std::string expected = "<b>My name is Jonas!</b>\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NestedIterating) {
  std::string input =
      "{{#inner}}{{foo}}{{#inner}}{{bar}}{{/inner}}{{/inner}}\n";
  mstch::node data = mstch::map{
      {"inner",
       mstch::array{mstch::map{
           {"foo", std::string{"foo"}},
           {"inner", mstch::array{mstch::map{{"bar", std::string{"bar"}}}}}}}}};

  std::string expected = "foobar\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Nesting) {
  std::string input{"{{#foo}}\n"
                    "  {{#a}}\n"
                    "    {{b}}\n"
                    "  {{/a}}\n"
                    "{{/foo}}\n"};
  mstch::node data = mstch::map{
      {"foo", mstch::array{mstch::map{{"a", mstch::map{{"b", 1}}}},
                           mstch::map{{"a", mstch::map{{"b", 2}}}},
                           mstch::map{{"a", mstch::map{{"b", 3}}}}}}};

  std::string expected{"    1\n"
                       "    2\n"
                       "    3\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NestingSameName) {
  std::string input = "{{#items}}{{name}}{{#items}}{{.}}{{/items}}{{/items}}\n";
  mstch::node data = mstch::map{
      {"items", mstch::array{mstch::map{{"name", std::string{"name"}},
                                        {"items", mstch::array{1, 2, 3, 4}}}}}};

  std::string expected = "name1234\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NullLookupArray) {
  std::string input{"{{#farray}}\n"
                    "{{#.}}{{#.}}{{.}} {{/.}}{{^.}}no twitter{{/.}}{{/.}}\n"
                    "{{/farray}}\n"};
  mstch::node data = mstch::map{
      {"name", std::string{"David"}},
      {"twitter", std::string{"@dasilvacontin"}},
      {"farray", mstch::array{
                     mstch::array{std::string{"Flor"}, std::string{"@florrts"}},
                     mstch::array{std::string{"Miquel"}, mstch::node{}},
                 }}};

  const std::string expected{"Flor @florrts \n"
                             "Miquel no twitter\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NullLookupObject) {
  std::string input{
      "{{#fobject}}\n"
      "{{name}}'s twitter: "
      "{{#twitter}}{{.}}{{/twitter}}{{^twitter}}unknown{{/twitter}}.\n"
      "{{/fobject}}\n"};
  mstch::node data = mstch::map{
      {"name", std::string{"David"}},
      {"twitter", std::string{"@dasilvacontin"}},
      {"fobject", mstch::array{mstch::map{{"name", std::string{"Flor"}},
                                          {"twitter", std::string{"@florrts"}}},
                               mstch::map{{"name", std::string{"Miquel"}},
                                          {"twitter", mstch::node{}}}}}};

  std::string expected{"Flor's twitter: @florrts.\n"
                       "Miquel's twitter: unknown.\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NullString) {
  std::string input{"Hello {{name}}\n"
                    "glytch {{glytch}}\n"
                    "binary {{binary}}\n"
                    "value {{value}}\n"};
  mstch::node data = mstch::map{{"name", std::string{"Elise"}},
                                {"glytch", true},
                                {"binary", false},
                                {"value", mstch::node{}}};

  std::string expected{"Hello Elise\n"
                       "glytch true\n"
                       "binary false\n"
                       "value \n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, NullView) {
  std::string input = "{{name}}'s friends: {{#friends}}{{name}}, {{/friends}}";
  mstch::node data =
      mstch::map{{"name", std::string{"Joe"}}, {"friends", mstch::node{}}};

  std::string expected = "Joe's friends: ";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, PartialArray) {
  std::string input = "{{>partial}}";
  mstch::node data =
      mstch::map{{"array", mstch::array{std::string{"1"}, std::string{"2"},
                                        std::string{"3"}, std::string{"4"}}}};
  std::string partial{"Here's a non-sense array of values\n"
                      "{{#array}}\n"
                      "  {{.}}\n"
                      "{{/array}}\n"};
  std::map<std::string, std::string> partials = {{"partial", partial}};

  std::string expected{"Here's a non-sense array of values\n"
                       "  1\n"
                       "  2\n"
                       "  3\n"
                       "  4\n"};
  std::string actual = mstch::render(input, data, partials);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, PartialArrayOfPartials) {
  std::string input{"Here is some stuff!\n"
                    "{{#numbers}}\n"
                    "{{>partial}}\n"
                    "{{/numbers}}\n"};
  mstch::node data = mstch::map{
      {"numbers", mstch::array{mstch::map{{"i", std::string{"1"}}},
                               mstch::map{{"i", std::string{"2"}}},
                               mstch::map{{"i", std::string{"3"}}},
                               mstch::map{{"i", std::string{"4"}}}}}};
  std::string partial = "{{i}}\n";
  std::map<std::string, std::string> partials = {{"partial", partial}};

  std::string expected{"Here is some stuff!\n"
                       "1\n"
                       "2\n"
                       "3\n"
                       "4\n"};
  std::string actual = mstch::render(input, data, partials);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, PartialArrayOfPartialsImplicit) {
  std::string input{"Here is some stuff!\n"
                    "{{#numbers}}\n"
                    "{{>partial}}\n"
                    "{{/numbers}}\n"};
  mstch::node data =
      mstch::map{{"numbers", mstch::array{std::string{"1"}, std::string{"2"},
                                          std::string{"3"}, std::string{"4"}}}};
  std::string partial = "{{.}}\n";
  std::map<std::string, std::string> partials = {{"partial", partial}};

  std::string expected{"Here is some stuff!\n"
                       "1\n"
                       "2\n"
                       "3\n"
                       "4\n"};
  std::string actual = mstch::render(input, data, partials);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, PartialEmpty) {
  std::string input{"hey {{foo}}\n"
                    "{{>partial}}\n"};
  mstch::node data = mstch::map{{"foo", 1}};
  std::string partial{};
  std::map<std::string, std::string> partials = {{"partial", partial}};

  std::string expected = "hey 1\n";
  std::string actual = mstch::render(input, data, partials);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, PartialTemplate) {
  std::string input{"<h1>{{title}}</h1>\n"
                    "{{>partial}}\n"};
  mstch::node data = mstch::map{
      {"title",
       mstch::lambda{[]() -> mstch::node { return std::string{"Welcome"}; }}},
      {"again",
       mstch::lambda{[]() -> mstch::node { return std::string{"Goodbye"}; }}},
  };
  std::string partial = "Again, {{again}}!\n";
  std::map<std::string, std::string> partials = {{"partial", partial}};

  std::string expected{"<h1>Welcome</h1>\n"
                       "Again, Goodbye!\n"};
  std::string actual = mstch::render(input, data, partials);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, RecursionWithSameNames) {
  std::string input{"{{ name }}\n"
                    "{{ description }}\n"
                    "\n"
                    "{{#terms}}\n"
                    "  {{name}}\n"
                    "  {{index}}\n"
                    "{{/terms}}\n"};
  mstch::node data = mstch::map{
      {"name", std::string{"name"}},
      {"description", std::string{"desc"}},
      {"terms",
       mstch::array{mstch::map{{"name", std::string{"t1"}}, {"index", 0}},
                    mstch::map{{"name", std::string{"t2"}}, {"index", 1}}}}};

  std::string expected{"name\n"
                       "desc\n"
                       "\n"
                       "  t1\n"
                       "  0\n"
                       "  t2\n"
                       "  1\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ReuseOfEnumerables) {
  std::string input{"{{#terms}}\n"
                    "  {{name}}\n"
                    "  {{index}}\n"
                    "{{/terms}}\n"
                    "{{#terms}}\n"
                    "  {{name}}\n"
                    "  {{index}}\n"
                    "{{/terms}}\n"};
  mstch::node data = mstch::map{
      {"terms",
       mstch::array{mstch::map{{"name", std::string{"t1"}}, {"index", 0}},
                    mstch::map{{"name", std::string{"t2"}}, {"index", 1}}}}};

  std::string expected{"  t1\n"
                       "  0\n"
                       "  t2\n"
                       "  1\n"
                       "  t1\n"
                       "  0\n"
                       "  t2\n"
                       "  1\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, SectionAsContext) {
  std::string input{"{{#a_object}}\n"
                    "  <h1>{{title}}</h1>\n"
                    "  <p>{{description}}</p>\n"
                    "  <ul>\n"
                    "    {{#a_list}}\n"
                    "    <li>{{label}}</li>\n"
                    "    {{/a_list}}\n"
                    "  </ul>\n"
                    "{{/a_object}}\n"};
  mstch::node data = mstch::map{
      {"a_object",
       mstch::map{
           {"title", std::string{"this is an object"}},
           {"description", std::string{"one of its attributes is a list"}},
           {"a_list",
            mstch::array{mstch::map{{"label", std::string{"listitem1"}}},
                         mstch::map{{"label", std::string{"listitem2"}}}}}}}};

  std::string expected{"  <h1>this is an object</h1>\n"
                       "  <p>one of its attributes is a list</p>\n"
                       "  <ul>\n"
                       "    <li>listitem1</li>\n"
                       "    <li>listitem2</li>\n"
                       "  </ul>\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, SectionFunctionsInPartials) {
  std::string input{"{{> partial}}\n"
                    "\n"
                    "<p>some more text</p>\n"};
  mstch::node data = mstch::map{
      {"bold", mstch::lambda{[](const std::string &text) -> mstch::node {
         return std::string{"<b>"} + text + std::string{"</b>"};
       }}}};
  std::string partial = "{{#bold}}Hello There{{/bold}}\n";
  std::map<std::string, std::string> partials = {{"partial", partial}};

  std::string expected{"<b>Hello There</b>\n"
                       "\n"
                       "<p>some more text</p>\n"};
  std::string actual = mstch::render(input, data, partials);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, StringAsContext) {
  std::string input{"<ul>\n"
                    "{{#a_list}}\n"
                    "  <li>{{a_string}}/{{.}}</li>\n"
                    "{{/a_list}}\n"
                    "</ul>"};
  mstch::node data =
      mstch::map{{"a_string", std::string{"aa"}},
                 {"a_list", mstch::array{std::string{"a"}, std::string{"b"},
                                         std::string{"c"}}}};

  std::string expected{"<ul>\n"
                       "  <li>aa/a</li>\n"
                       "  <li>aa/b</li>\n"
                       "  <li>aa/c</li>\n"
                       "</ul>"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, TwoInARow) {
  std::string input = "{{greeting}}, {{name}}!\n";
  mstch::node data = mstch::map{{"name", std::string{"Joe"}},
                                {"greeting", std::string{"Welcome"}}};

  std::string expected = "Welcome, Joe!\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, TwoSections) {
  std::string input{"{{#foo}}\n"
                    "{{/foo}}\n"
                    "{{#bar}}\n"
                    "{{/bar}}\n"};
  mstch::node data = mstch::map{};

  std::string expected{};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, Unescaped) {
  std::string input = "<h1>{{{title}}}</h1>\n";
  mstch::node data = mstch::map{{"title", mstch::lambda{[]() -> mstch::node {
                                   return std::string{"Bear > Shark"};
                                 }}}};

  std::string expected = "<h1>Bear > Shark</h1>\n";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, WhiteSpace) {
  std::string input{"{{tag1}}\n"
                    "\n"
                    "\n"
                    "{{tag2}}.\n"};
  mstch::node data = mstch::map{{"tag1", std::string{"Hello"}},
                                {"tag2", std::string{"World"}}};

  std::string expected{"Hello\n"
                       "\n"
                       "\n"
                       "World.\n"};
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}

TEST(RenderTest, ZeroView) {
  std::string input = "{{#nums}}{{.}},{{/nums}}";
  mstch::node data = mstch::map{{"nums", mstch::array{0, 1, 2}}};

  std::string expected = "0,1,2,";
  std::string actual = mstch::render(input, data);
  EXPECT_EQ(expected, actual);
}
