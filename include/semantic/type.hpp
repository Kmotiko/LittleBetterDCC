#ifndef DCC_TYPE_HPP
#define DCC_TYPE_HPP

#include<boost/variant/variant.hpp>

namespace dcc{
namespace semantic{
namespace type{


struct type{
public:
  type(){}
  virtual ~type() {}
  virtual std::string to_string() = 0;
};


struct builtin_type : public type{
public:
  builtin_type(std::string name) : name(name){}
  builtin_type() : type() {}

  virtual std::string to_string() {return name;}

  std::string name;
};

using builtin_type_ptr = std::shared_ptr<builtin_type>;

// for future implement...
using one_of_type = boost::variant<builtin_type_ptr>;

} // end of namespace type
} // end of namespace semantic
} // end of namespace dcc

#endif
