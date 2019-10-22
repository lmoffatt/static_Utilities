#ifndef QM_PACK_OPERATIONS_H
#define QM_PACK_OPERATIONS_H

#include <iostream>
#include <utility>
#include "mytypetraits.h"

template<class...> struct Cs{};
template<class...> class C{};

template <typename T> struct C<T> { typedef T type; };


template<class ...Ts>
std::istream& operator>>(std::istream& is,Cs<Ts...>)
{
  ((is>>Ts{}),...); return is;
}

template<class Id, class...Ids>
constexpr bool is_in_pack()
{
  return (std::is_same_v<Id,Ids >||...||false);
}

template<class Id, template<class...>class Cs,class...Ids>
constexpr bool is_in_pack(Id, Cs<Ids...>)
{
  return (std::is_same_v<Id,Ids >||...||false);
}



namespace impl {
template <template <typename...> class Cs, typename... Ts, typename T, std::size_t ...Is>
auto constexpr index_of_this_type(Cs<Ts...>, T, std::index_sequence<Is...>) {
  return ((std::is_same_v<Ts,T> ? Is : 0 )+...);
};
}

template <template <typename...> class Cs, template <typename> class C,typename... Ts, typename T>
auto constexpr index_of_this_type(Cs<T,Ts...> c, C<T> t) {
  return impl::index_of_this_type(c,t,std::index_sequence_for<Ts...>());
};


template <class T, T...I0, T...I1>
std::integer_sequence<T,I0...,I1...> operator<<(std::integer_sequence<T,I0...>, std::integer_sequence<T,I1...>) {return {};}

template <template <typename...> class Cs, typename... Ts, typename T, std::size_t ...Is>
auto constexpr pack_multindex(T,Cs<Ts...>, std::index_sequence<Is...>) {
  return (std::index_sequence<>{}|...|
          [](auto index){ if constexpr (std::is_same_v<Ts,T>)
      return index<<std::index_sequence<Is>{};
                                else return index;});
};

template <template <typename...> class Cs, typename... Ts, typename T, std::size_t ...Is>
auto constexpr pack_index(T,Cs<Ts...>, std::index_sequence<Is...>) {
  return ((std::is_same_v<Ts,T> ? Is : 0 )+...);
};


template <template <typename...> class Cs, typename... Ts, typename T, class=std::enable_if_t<is_in_pack<T,Ts...>(),int>>
auto constexpr pack_index(T,Cs<Ts...> ) {
  return pack_index(T{},Cs<Ts...>{},std::index_sequence_for<Ts...>());
};

template <typename T,template <typename...> class Cs, typename... Ts,  class=std::enable_if_t<is_in_pack<T,Ts...>(),int>>
auto constexpr pack_multindex(Cs<Ts...> ) {
  return pack_multindex(T{},Cs<Ts...>{},std::index_sequence_for<Ts...>());
};



template <template <typename...> class Cs, typename... Ts, typename... T>//, class=std::enable_if_t<(is_in_pack<T,Ts...>()&&...),int>>
auto constexpr pack_index_cs(Cs<T...>,Cs<Ts...> ) {
  return std::index_sequence<pack_index(T{},Cs<Ts...>{})...>{};
};


template<class, class> struct pack_transfer{};


template<template<class...>class Co,template<class...>class D, class...T, class...T0>
struct pack_transfer<Co<T...>,D<T0...>>
{
  typedef D<T0...,T...> type;
};

template<template<class...>class Co,template<template<class...>class,class...>class D, template<class...>class Tr,class...T>
struct pack_transfer<Co<T...>,D<Tr>>
{
  typedef D<Tr,T...> type;
};


template<class S, class D>
using transfer_t=typename pack_transfer<S,D>::type;

template <template <class> class , class> struct pack_apply;

template <template <class> class F_t, template<class...> class Cs, class... Ts>
struct pack_apply<F_t,Cs<Ts...>> { using type=Cs<F_t<Ts>...>;};


template <template <class> class F_t, class D> using
    pack_apply_t=typename pack_apply<F_t,D>::type;



template<class... Fs, class...Us>
static constexpr auto operator | (Cs<Fs...>,Cs<Us...>)
{
  return Cs<Fs...,Us...>{};
}

template<class...Ts>
struct pack_concatenation
{
  typedef decltype ((Ts{}|...)) type;
};
template<class...Ts>
using pack_concatenation_t=typename pack_concatenation<Ts...>::type;


template<class...Ts, class...Us, class... Tu>
struct pack_concatenation<std::tuple<Ts...>,std::tuple<Us...>,Tu...>
{
  using type=pack_concatenation_t<std::tuple<Ts...,Us...>,Tu...>;
};

template<class...Ts>
struct pack_concatenation<std::tuple<Ts...>>
{
  using type=std::tuple<Ts...>;
};




template<class One,class Two> struct pack_difference;

template<class T,class... Ts>
struct pack_difference<Cs<T,Ts...>,Cs<T,Ts...>>
{
  typedef Cs<> type;
};

template<class... Ts>
struct pack_difference<Cs<>,Cs<Ts...>>
{
  typedef Cs<> type;
};



template<class... Ts, class...Us>
struct pack_difference<Cs<Ts...>,Cs<Us...>>
{
  typedef pack_concatenation_t<
      std::conditional_t<
          (!is_in_pack<Ts,Us...>()),Cs<Ts>,Cs<>
          >...>
      type;
};

template<class One,class Two>
using pack_difference_t =typename pack_difference<One,Two>::type;

template<class One,class Two> struct pack_union;


template<class... Ts, class...Us>
struct pack_union<Cs<Ts...>,Cs<Us...>>
{
  typedef pack_concatenation_t<
      Cs<Ts...>,
      pack_difference_t<Cs<Us...>,Cs<Ts...>
                        >>
      type;
};

template<class One,class Two>
using pack_union_t =typename pack_union<One,Two>::type;


template<class... Ts, class T>
auto operator>>(Cs<Ts...>,Cs<T>){return Cs<Ts...>{};}

template<class... Ts, class T1, class T2, class... T2s>
auto operator>>(Cs<Ts...>,Cs<T1,T2,T2s...>){return Cs<Ts...,T1,T2,T2s...>{};}

template <class> struct pack_drop_back;
template <template<class...>class Cs,class...Ts>
struct pack_drop_back<Cs<Ts...>>
{
  typedef decltype ((...>> Cs<Ts>{})) type;
};

template <class CsTs>
using pack_drop_back_t=typename pack_drop_back<CsTs>::type;


constexpr bool is_contained_in(Cs<>, Cs<>)
{
  return  true;
}


template<class...Us>
constexpr bool is_contained_in(Cs<>, Cs<Us...>)
{
  return  true;
}

template<class...Us>
constexpr bool is_contained_in(Cs<Us...>, Cs<>)
{
  return  false;
}

template<class T, class...Ts,class U, class...Us>
constexpr bool is_contained_in(Cs<T,Ts...>, Cs<U,Us...>)
{
  if constexpr (!std::is_same_v<T, U>) return false;
  else
    return is_contained_in(Cs<Ts...>{},Cs<Us...>{});
}

template<class...> struct pack_until_this;


struct pack_end{};
template<class...Ts> struct Cs_end{};

template <class...Ts>
auto operator| (Cs<Ts...>, Cs<pack_end>){return Cs_end<Ts...>{};}
template <class...Ts, class...Us>
auto operator| (Cs_end<Ts...>, Cs<Us...>){return Cs_end<Ts...>{};}


template<class I, template <class...>class vec,class...Xs>
struct pack_until_this<I,vec<I,Xs...>>
{
  typedef vec<> type;
};

template<class I, template <class...>class vec,class...Xs>
struct pack_until_this<I,vec<Xs...>>
{
  typedef transfer_t<decltype ((std::conditional_t<std::is_same_v<I,Xs>,Cs<pack_end>,Cs<Xs>>{}|...)),vec<>> type;
};


template <class A, class B>
using pack_until_this_t=typename pack_until_this<A,B>::type;



template <class... Ts,class T>
constexpr auto operator||(Cs<Ts...>,Cs<T>)
{
  if constexpr (is_in_pack<T,Ts...>()) return Cs<Ts...>{};
  else return Cs<Ts...,T>{};

}

template<class > struct pack_unique;
template<class Ts>
using pack_unique_t=typename pack_unique<Ts>::type;


template<class...Ts> struct pack_unique<Cs<Ts...>> { using type=decltype ((...||Cs<Ts>{}));};

template<class...Ts> struct pack_unique<std::tuple<Ts...>> { using type=transfer_t<pack_unique_t<Cs<Ts...>>,std::tuple<>>;};






template<class...Ts0, class ...Ts2, std::size_t... I0, std::size_t... I1>
std::tuple<std::tuple<Ts0...>, std::tuple<Ts2...>>  distribute(Cs<Ts0...>,Cs<Ts2...>,std::tuple<Ts0...,Ts2...>&& tu,
                                                              std::index_sequence<I0...>, std::index_sequence<I1...>)
{
  return std::tuple(std::tuple<Ts0...>{std::get<I0>(std::move(tu))...},
                    std::tuple<Ts2...>{std::get<sizeof... (Ts0)+I1>(std::move(tu))...});
}

template<class...Ts0, class ...Ts2>
std::tuple<std::tuple<Ts0...>, std::tuple<Ts2...>>  distribute(Cs<Ts0...>,Cs<Ts2...>,std::tuple<Ts0...,Ts2...>&& tu)
{
  return distribute(Cs<Ts0...>{}, Cs<Ts2...>{},std::move(tu),std::index_sequence_for<Ts0...>{}, std::index_sequence_for<Ts2...>{});
}


template <class Fout,class Fin, class...Ts, class...Us, std::size_t...Is>
auto apply_twin(Fout&& fout,Fin&& fin, const std::tuple<Ts...>& t1,const std::tuple<Us...>& t2, std::index_sequence<Is...> )
{
  return std::invoke(std::forward<Fout>(fout),std::invoke(std::forward<Fin>(fin),std::get<Is>(t1), std::get<Is>(t2))...);
}

template <class Fout,class Fin, class...Ts, class...Us>
auto apply_twin(Fout&& fout,Fin&& fin, const std::tuple<Ts...>& t1,const std::tuple<Us...>& t2)
{
  return apply_twin(std::forward<Fout>(fout),std::forward<Fin>(fin),t1,t2,std::index_sequence_for<Ts...>());
}

#include <variant>

template<class ...>struct pack_to_column;

template<class ...Ts > using pack_to_column_t=typename pack_to_column<Ts...>::type;

template <class T> struct pack_to_column<T>{using type=T;};
template <class... T> struct pack_to_column<std::tuple<T...>>{using type=std::tuple<T...>;};


template <class... Ts> struct pack_to_column{using type=std::variant<Ts...>;};

template <class ...Tu, std::size_t I> struct pack_to_column<std::integral_constant<std::size_t,I>,Tu...>
{
  using type=typename transfer_t<pack_unique_t<Cs<std::tuple_element_t<I,Tu>...>>,pack_to_column<>>::type;
};

template <class ...Tu, std::size_t...Is> struct pack_to_column<std::index_sequence<Is...>,Tu...>
{
  using type=std::tuple<pack_to_column_t<std::integral_constant<std::size_t,Is>,Tu...>...>;
};



template <class...Ts,  class ...Tu> struct pack_to_column<std::tuple<Ts...>,Tu...>
{
  using type=pack_to_column_t<std::index_sequence_for<Ts...>,std::tuple<Ts...>,Tu...>;
};




#endif // QM_PACK_OPERATIONS_H
