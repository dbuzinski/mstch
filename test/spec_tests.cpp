#include "mstch/mstch.hpp"
#include "tools/cpp/runfiles/runfiles.h" // Bazel's runfiles library
#include "yaml-cpp/yaml.h"
#include <boost/variant/get.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <ostream>
#include <string>

using bazel::tools::cpp::runfiles::Runfiles;
using ::testing::ValuesIn;

// Define our parameter type based on the fields in the spec
struct SpecTestParam {
  std::string testName;
  std::string desc;
  std::string tmpl;
  std::string expected;
  mstch::node data;
  std::map<std::string, std::string> partials;
};

void PrintTo(const SpecTestParam &param, std::ostream *os) {
  *os << "Test name: " << param.testName;
}

class SpecTest : public ::testing::TestWithParam<SpecTestParam> {};

TEST_P(SpecTest, YamlTestCase) {
  const SpecTestParam &param = SpecTest::GetParam();
  std::string result = mstch::render(param.tmpl, param.data, param.partials);
  EXPECT_EQ(param.expected, result);
}

bool try_convert(const std::string &s, int &out_int) {
  try {
    size_t pos;
    int value = std::stoi(s, &pos);
    if (pos == s.size()) {
      out_int = value;
      return true;
    }
  } catch (...) {
  }
  return false;
}

bool try_convert(const std::string &s, double &out_double) {
  try {
    size_t pos;
    double value = std::stod(s, &pos);
    if (pos == s.size()) {
      out_double = value;
      return true;
    }
  } catch (...) {
  }
  return false;
}

bool try_convert(const std::string &s, bool &out_bool) {
  if (s == "true" || s == "True") {
    out_bool = true;
    return true;
  }
  if (s == "false" || s == "False") {
    out_bool = false;
    return true;
  }
  return false;
}

std::map<std::string, std::string> extract_partials(const YAML::Node &test) {
  std::map<std::string, std::string> partials;
  if (test["partials"]) {
    for (YAML::const_iterator it = test["partials"].begin();
         it != test["partials"].end(); ++it) {
      // Convert each partial's key and value to strings.
      std::string key = it->first.as<std::string>();
      std::string value = it->second.as<std::string>();
      partials[key] = value;
    }
  }
  return partials;
}

mstch::node convert_yaml_to_mstch(const YAML::Node &node) {
  switch (node.Type()) {
  case YAML::NodeType::Null:
  case YAML::NodeType::Undefined:
    return nullptr;

  case YAML::NodeType::Scalar: {
    std::string s = node.Scalar();
    // Attempt to convert to a bool first.
    bool b;
    if (try_convert(s, b))
      return b;  // Return a bool, not a string.

    // Then try converting to an int.
    int i;
    if (try_convert(s, i))
      return i;

    // Then try converting to a double.
    double d;
    if (try_convert(s, d))
      return d;

    // Otherwise, return as a string.
    return s;
  }

  case YAML::NodeType::Sequence: {
    mstch::array arr;
    for (std::size_t i = 0; i < node.size(); ++i)
      arr.push_back(convert_yaml_to_mstch(node[i]));
    return arr;
  }

  case YAML::NodeType::Map: {
    mstch::map m;
    for (auto it = node.begin(); it != node.end(); ++it) {
      std::string key = it->first.as<std::string>();
      m[key] = convert_yaml_to_mstch(it->second);
    }
    return m;
  }

  default:
    return {};
  }
}

std::vector<SpecTestParam> testCasesForFile(std::string filename) {
  std::vector<SpecTestParam> params;
  std::string error;
  std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest(&error));
  if (!error.empty()) {
    ADD_FAILURE() << "Runfiles error: " << error;
    return params;
  }
  // Locate the YAML file via runfiles.
  std::string yamlPath = runfiles->Rlocation(filename);

  YAML::Node config = YAML::LoadFile(yamlPath);
  // Ensure the YAML file contains a "tests" sequence.
  if (!config["tests"]) {
    ADD_FAILURE() << "No 'tests' key found in " << yamlPath;
    return params;
  }
  for (const auto &test : config["tests"]) {
    SpecTestParam param;
    param.testName = test["name"].as<std::string>();
    param.desc = test["desc"].as<std::string>();
    param.tmpl = test["template"].as<std::string>();
    param.expected = test["expected"].as<std::string>();
    param.data = convert_yaml_to_mstch(test["data"]);
    param.partials = extract_partials(test);
    params.push_back(param);
  }
  return params;
}

auto testCaseNames =
    [](const testing::TestParamInfo<SpecTest::ParamType> &info) {
      std::string name = info.param.testName;
      name.erase(std::remove_if(
                     name.begin(), name.end(),
                     [](auto const &c) -> bool { return !std::isalnum(c); }),
                 name.end());
      return name;
    };

// comments.yml
auto commentsTests = testCasesForFile("+_repo_rules+mustache_spec/spec-1.4.2/specs/comments.yml");
INSTANTIATE_TEST_SUITE_P(Comments, SpecTest, ValuesIn(commentsTests), testCaseNames);
// delimiters.yml
auto delimitersTests = testCasesForFile("+_repo_rules+mustache_spec/spec-1.4.2/specs/delimiters.yml");
INSTANTIATE_TEST_SUITE_P(Delimiters, SpecTest, ValuesIn(delimitersTests), testCaseNames);
//interpolation.yml
auto interpolationTests = testCasesForFile("+_repo_rules+mustache_spec/spec-1.4.2/specs/interpolation.yml");
INSTANTIATE_TEST_SUITE_P(Interpolation, SpecTest, ValuesIn(interpolationTests), testCaseNames);
// inverted.yml
auto invertedTests = testCasesForFile("+_repo_rules+mustache_spec/spec-1.4.2/specs/inverted.yml");
INSTANTIATE_TEST_SUITE_P(Inverted, SpecTest, ValuesIn(invertedTests), testCaseNames);
// partials.yml
auto partialsTests = testCasesForFile("+_repo_rules+mustache_spec/spec-1.4.2/specs/partials.yml");
INSTANTIATE_TEST_SUITE_P(Partials, SpecTest, ValuesIn(partialsTests), testCaseNames);
// sections.yml
auto sectionsTests = testCasesForFile("+_repo_rules+mustache_spec/spec-1.4.2/specs/sections.yml");
INSTANTIATE_TEST_SUITE_P(Sections, SpecTest, ValuesIn(sectionsTests), testCaseNames);
