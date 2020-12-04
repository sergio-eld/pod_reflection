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

int main()
{
    decltype(abc{}) abc1;
    decltype(abc{detail::convertible_to_any<0>()}) abc2;

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
    static_assert(detail::is_aggregate_initializable<abc, int, float, char, char, int, double, std::string>(), "Unexpected");
    static_assert(detail::is_aggregate_initializable<abc, abc>(), "Unexpected");

    // constexpr size_t argcAbc = detail::args_counter<abc>::value();

    return 0;
}
