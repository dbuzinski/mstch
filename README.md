# mstch - {{mustache}} templates in C++17 

![mstch logo](http://i.imgur.com/MRyStO5.png)

mstch is a complete implementation of [{{mustache}}](http://mustache.github.io/) 
templates using modern C++. It's compliant with [specifications](https://github.com/mustache/spec)
v1.4.2, including the lambda module.

[![GitHub version](https://badge.fury.io/gh/dbuzinski%2Fmstch.svg)](http://badge.fury.io/gh/dbuzinski%2Fmstch)
[![Build Status](https://github.com/dbuzinski/mstch/actions/workflows/ci.yml/badge.svg)](https://github.com/dbuzinski/mstch/actions/workflows/ci.yml)

## Supported features

mstch supports the complete feature set described in the `mustache(5)` [manpage](https://mustache.github.io/mustache.5.html):

 - JSON-like data structure using standard C++ types
 - variables, sections, inverted sections
 - partials
 - changing the delimiter
 - C++ lambdas
 - C++ objects as view models

## Basic usage

```c++
#include <iostream>
#include <mstch/mstch.hpp>

int main() {
  std::string view{"{{#names}}Hi {{name}}!\n{{/names}}"};
  mstch::map context{
    {"names", mstch::array{
      mstch::map{{"name", std::string{"Chris"}}},
      mstch::map{{"name", std::string{"Mark"}}},
      mstch::map{{"name", std::string{"Scott"}}},
    }}
  };
  
  std::cout << mstch::render(view, context) << std::endl;
  
  return 0;
}

```

The output of this example will be:

```html
Hi Chris!
Hi Mark!
Hi Scott!
```

### Data structure

The types in the example above, `mstch::array` and `mstch::map` are  actually 
aliases for standard types:

```c++
using map = std::map<const std::string, node>;
using array = std::vector<node>;
```

`mstch::node` is a `std::variant` type that can hold a `std::string`, `int`, 
`double`, `bool`, `mstch::lambda` or a `std::shared_ptr<mstch::object>` 
(see below), also a map or an array recursively. Essentially it works just like 
a JSON object.

Note that when using a `std::string` as value you must explicitly specify the 
type, since a `const char*` literal like `"foobar"` would be implicitly 
converted to `bool`. Alternatively you can use [C++14 string_literals](http://en.cppreference.com/w/cpp/string/basic_string/operator%22%22s)
if your compiler supports it.

## Advanced usage

### Partials

Partials can be passed in a `std::map` as the third parameter of the 
`mstch::render` function:

```c++
std::string view{"{{#names}}{{> user}}{{/names}}"};
std::string user_view{"<strong>{{name}}\n</strong>"};
mstch::map context{
  {"names", mstch::array{
    mstch::map{{"name", std::string{"Chris"}}},
    mstch::map{{"name", std::string{"Mark"}}},
    mstch::map{{"name", std::string{"Scott"}}},
  }}
};
  
std::cout << mstch::render(view, context, {{"user", user_view}}) << std::endl;
```

Output:

```html
<strong>Chris</strong>
<strong>Mark</strong>
<strong>Scott</strong>
```

### Lambdas

C++ lambdas can be used to add logic to your templates. Like a
`const char*` literal, lambdas can be implicitly converted to `bool`, so they
must be wrapped in a `mstch::lambda` object when used in a `mstch::node`. The 
lambda expression passed to `mstch::lambda` must itself return a `mstch::node`.
The returned node will be rendered to a string, then it will be parsed as a
template.

The lambda expression accepts either no parameters:

```c++
std::string view{"Hello {{lambda}}!"};
mstch::map context{
  {"lambda", mstch::lambda{[]() -> mstch::node {
    return std::string{"World"};
  }}}
};

std::cout << mstch::render(view, context) << std::endl;
```

Output:

```html
Hello World!
```

Or it accepts a `const std::string&` that gets the unrendered literal block:

```c++
std::string view{"{{#bold}}{{yay}} :){{/bold}}"};
mstch::map context{
  {"yay", std::string{"Yay!"}},
  {"bold", mstch::lambda{[](const std::string& text) -> mstch::node {
    return "<b>" + text + "</b>";
  }}}
};

std::cout << mstch::render(view, context) << std::endl;
```

Output:

```html
<b>Yay! :)</b>
```

### Objects

Custom objects can also be used as context for rendering templates. The class 
must inherit from `mstch::object`, and register it's exported methods with
`register_methods`. Exported methods must have the return type of `mstch::node`.
Objects must be created as a `std::shared_ptr`.

```c++
class example: public mstch::object {
 public:
  example(): m_value(1) {
    register_methods(this, {
      {"count", &example::count},
      {"names", &example::names}  
    });
  }
  
  mstch::node count() {
    return m_value++;
  }
  
  mstch::node names() {
    return mstch::array{
      std::string{"Chris"}, std::string{"Mark"}, std::string{"Scott"}};
  }
  
 private:
  int m_value;
};

std::string view{"{{#names}}<b>{{count}}</b>: {{.}}\n{{/names}}"};
const auto context = std::make_shared<example>();

std::cout << mstch::render(view, context) << std::endl;
```

Output:

```html
<b>1</b>: Chris
<b>2</b>: Mark
<b>3</b>: Scott
```

### Custom escape function

By default, mstch uses HTML escaping on the output, as per specification. This
is not useful if your output is not HTML, so mstch provides a way to supply
your own escape implementation. Just assign any callable object to the static
`mstch::config::escape`, which is an initially empty
`std::function<std::string(const std::string&)>`. 

For example you can turn off escaping entirely with a lambda:

```c++
mstch::config::escape = [](const std::string& str) -> std::string {
  return str;
};
```

## Requirements

 - A C++17 compliant compiler. Currently tested with:
   - GCC 7.0+
   - Clang 5.0+
   - MSVC 2017+
 - Bazel 7.0+ for building

## Using mstch in your project

If you are using Bazel, you can include mstch in your project by adding it as a dependency in your `MODULE.bazel` file:

```python
bazel_dep(name = "mstch", version = "2.0.0")
```

Then in your `BUILD` file:

```python
cc_library(
    name = "your_library",
    deps = ["@mstch//:mstch"],
)
```

### Building and Testing

To build the library and run tests:

```bash
# Build the library
bazel build //:mstch

# Run tests
bazel test //:mstch_test

# Run benchmarks
bazel run //:benchmark
```

### Running Benchmarks

The project includes Google Benchmark for performance testing. To run the benchmarks:

```bash
# Run with optimizations enabled
bazel build -c opt //:benchmark
./bazel-bin/benchmark --benchmark_repetitions=5 --benchmark_report_aggregates_only=true
```

The benchmarks will provide detailed timing information for various template operations.

## License

mstch is licensed under the [MIT license](https://github.com/no1msd/mstch/blob/master/LICENSE).
