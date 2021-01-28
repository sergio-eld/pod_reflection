#include <pod_reflection/pod_reflection.h>

#include <string>
#include <iostream>

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
    none none{};
    eld::for_each<TupleFeed>(none, Printer());
    eld::detail::make_index_sequence<0> zeroSeq{};

    static_assert(std::is_same<eld::detail::make_index_sequence<0>, eld::detail::index_sequence<>>::value, "");

    return 0;
}
