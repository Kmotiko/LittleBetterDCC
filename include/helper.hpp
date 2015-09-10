#ifndef DCC_HELPER_HPP
#define DCC_HELPER_HPP

#include <boost/variant.hpp>
#include <boost/optional.hpp>

namespace dcc{
namespace helper{

// {{{
template<typename Result, typename Function>
struct wrapped_function{
  Function f;
  wrapped_function(Function f) : f(f){}
  using result_type = Result;

  template<typename T>
  result_type operator()(T & val){
    return f(val);
  }

  template<typename T>
  result_type operator()(T const& val){
    return f(val);
  }
};


template<typename Result, typename Function>
wrapped_function<Result, Function> make_wrapped_function(Function f)
{
  return wrapped_function<Result, Function>(f);
}


template<typename T, typename Variant>
boost::optional<T> get(Variant &v){
  if(v.type() == typeid(T))
    return boost::get<T>(v);
  else
    return boost::none;
}
// }}}


}
}

#endif
