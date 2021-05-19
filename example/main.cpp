#include <pod_reflection/pod_reflection.h>

#include <string>
#include <iostream>
#include <atomic>

struct abc
{
    int a;
    float b;
    char c,
            d;
    int e;
    double f;
    std::string g;
};

struct dce
{
    int a;
    double b;
    abc c;
};

struct none
{
};

// This might be an endian converting class!
struct Printer
{
    template<typename T>
    void operator()(const T &t)
    {
        std::cout << t << " ";
    }
};

template<size_t I>
using array_i = std::array<std::ptrdiff_t, I>;

enum class Enoom : uint8_t
{
    foo = 42
};

struct with_enum
{
    Enoom enoom;
};

template <typename T>
using is_fundamental = std::is_fundamental<T>;

int main()
{
    std::cout << "struct abc has " << eld::pod_size<abc>() << " elements" << std::endl;
    std::cout << "struct dce has " << eld::pod_size<dce>() << " elements" << std::endl;
    std::cout << "struct none has " << eld::pod_size<none>() << " elements" << std::endl;

    using TupleFeed = eld::extend_feed<std::string>;

    if (std::is_same<std::string, eld::pod_element_t<6, abc, TupleFeed>>::value)
        std::cout << "6th element in struct abc is std::string" << std::endl;

    abc toPrint{4, 8, 'o', 'c', 15, 16.2342, "eanic"};


    eld::for_each<TupleFeed>(toPrint, Printer());
    std::cout << std::endl;
    none n{};
    eld::for_each<TupleFeed>(n, Printer());
    eld::detail::make_index_sequence<0> zeroSeq{};

    //////////////////////////////////////////////////////////
    // calculating offsets!
    //////////////////////////////////////////////////////////

    // empty offset
//    constexpr auto emptyArray0 = eld::detail::get_pod_offsets<none, TupleFeed>(array_i<eld::pod_size<none>::value>(),
//            eld::detail::is_equal<0, eld::pod_size<none>::value>());
//    static_assert(std::is_same<decltype(emptyArray0), const array_i<0>>(), "");
//
//    constexpr auto emptyArray1 = eld::detail::get_pod_offsets<none, TupleFeed>();
//    static_assert(std::is_same<decltype(emptyArray1), const array_i<0>>(), "");
//
//    static_assert(std::is_same<eld::detail::make_index_sequence<0>, eld::detail::index_sequence<>>::value, "");
//
//    constexpr auto abcOffsets = eld::detail::get_pod_offsets<abc, TupleFeed>();


    using expected_res_array_t = const std::array<std::ptrdiff_t, eld::pod_size<abc>::value>;
//    static_assert(std::is_same<expected_res_array_t, decltype(abcOffsets)>(), "Offsets arrays types mismatch!");


    decltype(with_enum{eld::detail::explicitly_convertible<uint8_t, eld::ignore_enums_t>()}) s;
    (void) s;
    static_assert(std::is_same<typename std::underlying_type<Enoom>::type,
            eld::pod_element_t<0, with_enum, TupleFeed, eld::ignore_enums_t>>(), "");


    constexpr auto withEnumOffsets = eld::detail::get_pod_offsets<with_enum, TupleFeed, eld::ignore_enums_t>();
    static_assert(withEnumOffsets[0] == 0, "");

    static_assert(eld::is_valid_pod<TupleFeed, with_enum, eld::ignore_enums_t>(), "");

    with_enum withEnum{Enoom::foo};

    eld::for_each(withEnum, [](std::underlying_type<Enoom>::type v)
    {
        std::cout << +v << std::endl;
    }, eld::ignore_enums);

    eld::detail::filter<TupleFeed, std::is_fundamental>{};

    static_assert(std::is_same<std::tuple<int>, eld::detail::append_if_t<std::tuple<>, int, std::is_fundamental>>(),
                  "");
    eld::detail::append_if<std::tuple<>, int, std::is_fundamental>{};

    using tuple_list_t = std::tuple<std::string, int, none, double, char, abc, bool>;
    using tuple_found_expected_t = std::tuple<int, double, char, bool>;
    using tuple_filtered_t = eld::detail::filter_t<tuple_list_t, std::is_fundamental>;

    static_assert(std::is_same<tuple_filtered_t, tuple_found_expected_t>(), "");

    static_assert(std::is_empty<std::tuple<>>(), "");

    using found_int_t = eld::detail::find_first_t<tuple_list_t, std::is_fundamental>;
    static_assert(std::is_same<found_int_t, int>(), "");


    return 0;
}
