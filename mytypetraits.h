#ifndef MYTYPETRAITS_H
#define MYTYPETRAITS_H
#include <type_traits>
#include <utility>
#include <array>
#include <string>
#include <iomanip>
#include <cstring>
#include <functional>
#include <qm_static_string.h>

//namespace qm {


template <class T>
auto operator<<(std::ostream& os, T x)->decltype (x.className,os)
{
  return os<<x.className.c_str();
}

template <class T>
auto operator>>(std::istream& is, T x)->decltype (x.className,is)
{
  std::string s;
  is>>s;
  std::string name(x.className.c_str());
  if (s==name)
  {
    return is;
  }
  else
  {
    is.setstate(std::ios::failbit);
    return is;
   }
}


template <class ...T>
std::istream& operator>>(std::istream& is, std::tuple<T...>& tu)
{
   std::apply([&is](auto&... e){((
                                       is>>e
                                       ),...);},tu);

   return is;
}


template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& me)
{
  os<<"  [";
  for (auto& x:me)
    os<<x<<" ";
  os<<"]   ";

  return os;
}

template<template<class...>class, class>
struct is_this_template_class
{
  static constexpr bool value=false;
};



template<template<class ...>class V, class...Ts>
struct is_this_template_class<V,V<Ts...>>
{
  static constexpr bool value=true;
};



template<class F, class T>
auto operator |(F&& f, T&& t)->decltype (std::invoke(f,t))
{
  return std::invoke(std::forward<F>(f),std::forward<T>(t));
}

template<class F, class T>
auto operator |(T&& t , F&& f)->decltype (std::invoke(f,t))
{
  return std::invoke(std::forward<F>(f),std::forward<T>(t));
}





//} // namespace qm


#endif // MYTYPETRAITS_H
