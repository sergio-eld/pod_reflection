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
{};

// This might be an endian converting class!
struct Printer
{
    template <typename T>
    void operator()(const T& t)
    {
        std::cout << t << " ";
    }
};

template <size_t I>
using array_i = std::array<std::ptrdiff_t, I>;

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
    constexpr auto emptyArray0 = eld::detail::get_pod_offsets<none, TupleFeed>(array_i<eld::pod_size<none>::value>(),
            eld::detail::is_equal<0, eld::pod_size<none>::value>());
    static_assert(std::is_same<decltype(emptyArray0), const array_i<0>>(), "");

    constexpr auto emptyArray1 = eld::detail::get_pod_offsets<none, TupleFeed>();
    static_assert(std::is_same<decltype(emptyArray1), const array_i<0>>(), "");

    static_assert(std::is_same<eld::detail::make_index_sequence<0>, eld::detail::index_sequence<>>::value, "");

    constexpr auto abcOffsets = eld::detail::get_pod_offsets<abc, TupleFeed>();


    using expected_res_array_t = const std::array<std::ptrdiff_t, eld::pod_size<abc>::value>;
//    static_assert(std::is_same<expected_res_array_t, decltype(abcOffsets)>(), "Offsets arrays types mismatch!");

    constexpr auto appendArrays = eld::detail::append_arrays(std::array<int, 4>{4, 8, 15, 16},
                                                             std::array<int, 2>{23, 42});

    return 0;
}
