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

    constexpr bool f = detail::is_aggregate_initializable_from_n_args<abc, 6>();

    constexpr size_t zero = accumulate_decrement(tag_s<0>(), 16);
    constexpr size_t sum = accumulate_decrement(tag_s<5>(), 5);

    constexpr size_t sumIncr = sum_increment(tag_s<0>(), 0);

    constexpr size_t fuck = sum_increment(tag_s<16>(), std::false_type());
    constexpr size_t you = sum_increment(tag_s<0>(), std::true_type());

    decltype(abc{}) abc1;
    decltype(abc{detail::implicitly_convertible()}) abc2;

    static_assert(std::is_same<std::tuple<int, double>, detail::tuple_push_back<std::tuple<int>, double>>::value,
                  "Failed to push_back to tuple");

//    static_assert(detail::is_aggregate_initializable_from_tuple<abc, std::tuple<int>>(), "Unexpected");

    constexpr bool b = detail::is_aggregate_initializable_from_tuple<abc, std::tuple<int>>();
    std::cout << b << std::endl;
    static_assert(detail::is_aggregate_initializable<abc>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, int>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, int, float>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, int, float, char>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, int, float, char, char>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, int, float, char, char, int>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, int, float, char, char, int, double>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, int, float, char, char, int, double, std::string>(),
                  "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, abc>(), "Unexpected");

    static_assert(detail::is_aggregate_initializable<abc, detail::implicitly_convertible, detail::implicitly_convertible>(),
                  "Unexpected");

    static_assert(eld::aggregate_args_counter<abc>() == 7, "Invalid arguments number!");
    static_assert(eld::aggregate_args_counter<none>() ==
                  std::numeric_limits<size_t>::max(), "Invalid arguments number!");

    static_assert(eld::detail::is_pod_member_initializable_from_t<abc, int, 0>(),
            "Unexpected result");
    static_assert(eld::detail::is_pod_member_initializable_from_t<abc, float, 1>(),
                  "Unexpected result");
    static_assert(eld::detail::is_pod_member_initializable_from_t<abc, std::string, 6>(),
                  "Unexpected result");

    static_assert(!eld::detail::is_pod_member_initializable_from_t<abc, void, 0>(),
                  "Unexpected result");

    return 0;
}
