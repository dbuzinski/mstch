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

  template<class T>
  std::string operator()(const T&) const {
    return "";
  }

  std::string operator()(const int& value) const {
    return std::to_string(value);
  }

  std::string operator()(const double& value) const {
    std::stringstream ss;
    ss << value;
    return ss.str();
  }

  std::string operator()(const bool& value) const {
    return value ? "true" : "false";
  }

  std::string operator()(const lambda& value) const {
    std::string lambda_result = value([this](const mstch::node& n) -> std::string {
      render_node no_escape_visitor(m_ctx, flag::none);
      return std::visit(no_escape_visitor, n);
    });
    
    if (lambda_result.find("{{") != std::string::npos) {
      template_type interpreted{lambda_result};
      return render_context::push(m_ctx).render(interpreted);
    } else {
      return (m_flag == flag::escape_html) ? html_escape(lambda_result) : lambda_result;
    }
  }

  std::string operator()(const std::string& value) const {
    return (m_flag == flag::escape_html) ? html_escape(value) : value;
  }

 private:
  render_context& m_ctx;
  flag m_flag;
};

}
