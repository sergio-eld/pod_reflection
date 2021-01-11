#include <pod_reflection/pod_reflection.h>

#include <string>

struct abc
{
    int a;// = 4;
    float b;// = 8.15f;        //vs 17 express does not allow implicit conversion from double
    char c,// = 'c',
    d;// = 'd';
    int e;// = 16;
    double f;// = 23.42;
    std::string g;// = "oceanic";
};

struct dce
{
    int a;// = 4;
    double b;// = 3.18;
    abc c;//{};
};

using namespace eld;

#include <iostream>

struct none
{
    none(void *)
    {}
};

template<size_t> struct tag_s
{
};

constexpr size_t accumulate_decrement(tag_s<0>, size_t sum)
{
    return sum;
}

template<size_t i>
constexpr size_t accumulate_decrement(tag_s<i>, size_t sum)
{
    return sum + accumulate_decrement(tag_s<i - 1>(), i - 1);
}

constexpr size_t sum_increment(tag_s<5>, size_t sum)
{
    return sum;
}

template<size_t i>
constexpr size_t sum_increment(tag_s<i>, size_t sum)
{
    return sum + sum_increment(tag_s<i + 1>(), i + 1);
}


template<size_t i>
constexpr size_t sum_increment(tag_s<i>, std::false_type)
{
    return i - 1;
}

template<size_t i>
constexpr size_t sum_increment(tag_s<i>, std::true_type)
{
    return sum_increment(tag_s<i + 1>(), std::integral_constant<bool, (i < 5)>());
}


int main()
{

    detail::make_index_sequence<5> sdgb;

    constexpr bool f = detail::is_aggregate_initialisable_from_n_args<abc, 6>();

    constexpr size_t zero = accumulate_decrement(tag_s<0>(), 16);
    constexpr size_t sum = accumulate_decrement(tag_s<5>(), 5);

    constexpr size_t sumIncr = sum_increment(tag_s<0>(), 0);

    constexpr size_t fuck = sum_increment(tag_s<16>(), std::false_type());
    constexpr size_t you = sum_increment(tag_s<0>(), std::true_type());

    decltype(abc{}) abc1;
    decltype(abc{detail::implicitly_convertible()}) abc2;

//    static_assert(std::is_same<std::tuple<int, double>, detail::tuple_push_back<std::tuple<int>, double>>::value,
//                  "Failed to push_back to tuple");
//
////    static_assert(detail::is_aggregate_initialisable_from_tuple<abc, std::tuple<int>>(), "Unexpected");
//
//    constexpr bool b = detail::is_aggregate_initialisable_from_tuple<abc, std::tuple<int>>();
//    std::cout << b << std::endl;
    static_assert(detail::is_aggregate_initialisable<abc>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char, int>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char, int, double>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char, int, double, std::string>(),
                  "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, abc>(), "Unexpected");

    static_assert(
            detail::is_aggregate_initialisable<abc, detail::implicitly_convertible, detail::implicitly_convertible>(),
            "Unexpected");

    static_assert(eld::pod_size<abc>() == 7, "Invalid arguments number!");
    static_assert(eld::pod_size<none>() ==
                  std::numeric_limits<size_t>::max(), "Invalid arguments number!");

    static_assert(eld::detail::is_pod_member_initialisable_from_t<abc, int, 0>(),
                  "Unexpected result");
    static_assert(eld::detail::is_pod_member_initialisable_from_t<abc, float, 1>(),
                  "Unexpected result");
    static_assert(eld::detail::is_pod_member_initialisable_from_t<abc, std::string, 6>(),
                  "Unexpected result");

    static_assert(!eld::detail::is_pod_member_initialisable_from_t<abc, void, 0>(),
                  "Unexpected result");

    using TupleFeedAbc = std::tuple<int, float, double, char, std::string>;

//    static_assert(eld::detail::tuple_index_from_pod_member<0, abc, std::tuple<int>>::value() == 0,
//                  "");
//    static_assert(eld::detail::tuple_index_from_pod_member<0, abc, std::tuple<double, int>>::value() == 1,
//                  "");
//    static_assert(eld::detail::tuple_index_from_pod_member<0, abc, std::tuple<double>>::value() == (size_t) -1,
//                  "");
//
//    static_assert(eld::detail::tuple_index_from_pod_member<1, abc, std::tuple<float>>::value() == 0,
//                  "");
//    static_assert(eld::detail::tuple_index_from_pod_member<1, abc, std::tuple<int, float>>::value() == 1,
//                  "");
//    static_assert(eld::detail::tuple_index_from_pod_member<1, abc, std::tuple<double>>::value() == (size_t) -1,
//                  "");

    // successful search

    constexpr size_t int_indx_0 =
            eld::detail::tuple_index_from_pod_member<0, abc, std::tuple<int>>::value();
    static_assert(int_indx_0 == 0, "");
    static_assert(std::is_same<int, eld::pod_element_t<0, abc, std::tuple<int>>>::value,
            "");

    constexpr size_t int_indx_1 =
            eld::detail::tuple_index_from_pod_member<0, abc, std::tuple<double, int>>::value();
    static_assert(int_indx_1 == 1, "");
    static_assert(std::is_same<int, eld::pod_element_t<0, abc, std::tuple<double, int>>>::value,
                  "");

    constexpr size_t float_indx =
        eld::detail::tuple_index_from_pod_member<1, abc, std::tuple<int, float>>::value();
//    constexpr size_t float_indx_notfound =
//        eld::detail::tuple_index_from_pod_member<1, abc, std::tuple<int>>::value();

    static_assert(std::is_same<std::string,
            eld::pod_element_t<6, abc, TupleFeedAbc>>::value, "Failed to deduce std::string in abc");

    return 0;
}
