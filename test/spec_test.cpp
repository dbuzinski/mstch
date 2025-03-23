#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include <gtest/gtest.h>
#include "json/json.h"

#include "test/data/specs_lambdas.hpp"
#include "mstch/mstch.hpp"

using ::testing::ValuesIn;

mstch::node json_to_mstch(const Json::Value &value) {
  if (value.isBool()) {
    return value.asBool();
  } else if (value.isInt()) {
    return value.asInt();
  } else if (value.isDouble()) {
    return value.asDouble();
  } else if (value.isString()) {
    return value.asString();
  } else if (value.isArray()) {
    mstch::array arr;
    for (Json::ArrayIndex i = 0; i < value.size(); ++i) {
      arr.push_back(json_to_mstch(value[i]));
    }
    return arr;
  } else if (value.isObject()) {
    mstch::map m;
    std::vector<std::string> keys = value.getMemberNames();
    for (const auto &key : keys) {
      m[key] = json_to_mstch(value[key]);
    }
    return m;
  }
  return {};
}

struct SpecTestParam {
  std::string name;
  std::string tmpl;
  std::string expected;
  mstch::node data;
  std::map<std::string, std::string> partials;

  std::string get_test_name() const {
    std::string name = this->name;
    name.erase(
        std::remove_if(name.begin(), name.end(),
                       [](auto const &c) -> bool { return !std::isalnum(c); }),
        name.end());
    return name;
  }

  static std::vector<SpecTestParam> from_json_file(const std::string &file_name) {
    std::vector<SpecTestParam> params;

    std::ifstream ifs(file_name, std::ios_base::binary);
    if (!ifs.is_open()) {
      throw std::runtime_error("Could not open file: " + file_name);
    }
    Json::Value root;
    ifs >> root;

    for (const Json::Value &test : root["tests"]) {
      SpecTestParam param;
      param.name = test["name"].asString();
      param.tmpl = test["template"].asString();
      param.expected = test["expected"].asString();
      param.data = json_to_mstch(test["data"]);
      std::map<std::string, std::string> partials;
      for (const auto &key : test["partials"].getMemberNames()) {
        partials[key] = test["partials"][key].asString();
      }
      param.partials = partials;
      params.push_back(param);
    }

    return params;
  }
};

void PrintTo(const SpecTestParam &param, std::ostream *os) {
  *os << "Test name: " << param.name;
}

class SpecTest : public ::testing::TestWithParam<SpecTestParam> {};

TEST_P(SpecTest, YamlTestCase) {
  const SpecTestParam &param = SpecTest::GetParam();
  std::string result = mstch::render(param.tmpl, param.data, param.partials);
  EXPECT_EQ(param.expected, result);
}

auto name_generator =
    [](const testing::TestParamInfo<SpecTest::ParamType> &info) {
      return info.param.get_test_name();
    };

INSTANTIATE_TEST_SUITE_P(
    Comments, SpecTest,
    ValuesIn(SpecTestParam::from_json_file("test/spec/specs/comments.json")),
    name_generator);

INSTANTIATE_TEST_SUITE_P(
    Delimiters, SpecTest,
    ValuesIn(SpecTestParam::from_json_file("test/spec/specs/delimiters.json")),
    name_generator);

INSTANTIATE_TEST_SUITE_P(
    Interpolation, SpecTest,
    ValuesIn(SpecTestParam::from_json_file("test/spec/specs/interpolation.json")),
    name_generator);

INSTANTIATE_TEST_SUITE_P(
    Inverted, SpecTest,
    ValuesIn(SpecTestParam::from_json_file("test/spec/specs/inverted.json")),
    name_generator);

INSTANTIATE_TEST_SUITE_P(
    Partials, SpecTest,
    ValuesIn(SpecTestParam::from_json_file("test/spec/specs/partials.json")),
    name_generator);

INSTANTIATE_TEST_SUITE_P(
    Sections, SpecTest,
    ValuesIn(SpecTestParam::from_json_file("test/spec/specs/sections.json")),
    name_generator);
