// A simple uri parser
// No user:pass judge

#pragma once

#include <charconv>
#include <cstdint>
#include <exception>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace xihale{
namespace net{
namespace uri{

  static const std::unordered_map<std::string_view, std::string_view> defaultPorts{
    {"http", "80"},
    {"https", "443"},
  };

  struct uri;
  static uri parse(std::string_view);

  class uri{
  public:
    using value_type = std::string_view;
    value_type scheme="http";
    value_type host;
    std::uint16_t port;
    // std::string_view authority; // unimplemented
    value_type path="/"; // included query and fragment
    // std::string_view query="";
    // std::string_view fragment="";

    uri() = default;
    uri(const uri &) = default;
    uri(uri &&) = default;
    uri(value_type scheme, value_type host, std::uint16_t port, value_type path)
            : scheme(scheme), host(host), port(port), path(path) {}
    template<typename value_type>
    uri(const value_type &__uri):uri(parse(__uri)){};
  };

  static uri parse(std::string_view __uri){
    const auto &npos=std::string_view::npos;
    
    uri res;

    // trim space
    auto pos=__uri.find_first_not_of(' ');
    if(pos!=npos) __uri.remove_prefix(pos);
    pos=__uri.find_last_not_of(' ');
    if(pos!=npos) __uri.remove_suffix(__uri.length()-pos-1);

    auto scheme=__uri.find_first_of("://");
    auto path=__uri.find_first_of('/');
    if(!(scheme==npos || scheme>=path)){ // not Protocol omitted
      res.scheme=__uri.substr(0, scheme);
      __uri.remove_prefix(scheme+3);
      path=__uri.find_first_of('/');
    }

    auto port=__uri.find_first_of(':');
    if(port==std::string_view::npos){
      port=(path==npos)?__uri.length():path;
      const auto &port=defaultPorts.find(res.scheme)->second;
      std::from_chars(port.data(), port.data()+port.length(), res.port);
    }
    else std::from_chars(__uri.data()+port+1, __uri.data()+(path==npos?__uri.length()-1:path)-port-1, res.port);
    res.host=__uri.substr(0, port);
    if(path!=npos) res.path=__uri.substr(path);

    return res;
  }

};
};
};

