#pragma once

#include <sstream>

#include "render_context.hpp"
#include "mstch/mstch.hpp"
#include "utils.hpp"

namespace mstch {

class render_node {
public:
  enum class flag { none, escape_html };
  render_node(render_context& ctx, flag p_flag = flag::none):
    m_ctx(ctx), m_flag(p_flag)
  {
  }

  template<typename T>
  std::string operator()(const T& value) const {
    if constexpr(std::is_same_v<T, long long> || std::is_same_v<T, int>) {
      return std::to_string(value);
    } else if constexpr(std::is_same_v<T, double>) {
      std::stringstream ss;
      ss << value;
      return ss.str();
    } else if constexpr(std::is_same_v<T, bool>) {
      return value ? "true" : "false";
    } else if constexpr(std::is_same_v<T, lambda>) {
      std::string lambda_result = value([this](const mstch::node& n) {
        return mstch::visit(render_node(m_ctx), n);
      });
      
      // Let template_type handle the parsing - it will tokenize if it contains mustache tags
      template_type interpreted{lambda_result};
      auto rendered = render_context::push(m_ctx).render(interpreted);
      return (m_flag == flag::escape_html) ? html_escape(rendered) : rendered;
    } else if constexpr(std::is_same_v<T, std::string>) {
      return (m_flag == flag::escape_html) ? html_escape(value) : value;
    }
    return {};
  }

private:
  render_context& m_ctx;
  flag m_flag;
};

}
