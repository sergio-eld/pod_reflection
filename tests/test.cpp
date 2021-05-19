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

#pragma pack(push, 1)
struct packed
{
    char a;
    int b;
};
#pragma pack(pop)

struct invalid_pod
{
    int a : 2;
    int b : 2;
    char c;
};

struct WithArray
{
    int a;
    int b[24];
    int c;
};

struct S {
    int x;
    struct Foo {
        int i;
        int j;
        int a[3];
    } b;
};

int main()
{
    std::cout << "struct abc has " << eld::pod_size<abc>() << " elements" << std::endl;
    std::cout << "struct dce has " << eld::pod_size<dce>() << " elements" << std::endl;
    std::cout << "struct none has " << eld::pod_size<none>() << " elements" << std::endl;

    std::cout << eld::detail::count_args<dce>(eld::detail::is_aggregate_initialisable<dce>()) <<
            std::endl;
    std::cout << eld::detail::count_args<none>(eld::detail::is_aggregate_initialisable<none>()) <<
              std::endl;
    std::cout << eld::pod_size<abc>() << std::endl;
    std::cout << eld::pod_size<none>() << std::endl;


    detail::make_index_sequence<5> sdgb;
    (void)sdgb;

    decltype(abc{}) abc1;
    decltype(abc{detail::implicitly_convertible()}) abc2;

    static_assert(detail::is_aggregate_initialisable<abc>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char, int>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char, int, double>(), "Unexpected");
    static_assert(detail::is_aggregate_initialisable<abc, int, float, char, char, int, double, std::string>(),
                  "Unexpected");
    // static_assert(detail::is_aggregate_initialisable<abc, abc>(), "Unexpected");

    static_assert(
            detail::is_aggregate_initialisable<abc, detail::implicitly_convertible, detail::implicitly_convertible>(),
            "Unexpected");

    static_assert(eld::pod_size<abc>() == 7, "Invalid arguments number!");
    static_assert(eld::pod_size<none>() == 0, "Invalid arguments number!");


    static_assert(std::is_same<abc, decltype(abc{eld::detail::explicitly_convertible<int>()})>::value, "");
    static_assert(eld::detail::is_pod_member_initialisable_from_t<abc, int, 0>(),
                  "Unexpected result");
    static_assert(eld::detail::is_pod_member_initialisable_from_t<abc, float, 1>(),
                  "Unexpected result");
    static_assert(eld::detail::is_pod_member_initialisable_from_t<abc, std::string, 6>(),
                  "Unexpected result");

    static_assert(!eld::detail::is_pod_member_initialisable_from_t<abc, void, 0>(),
                  "Unexpected result");

    using TupleFeedAbc = eld::extend_feed<std::string>;//std::tuple<int, float, double, char, std::string>;

    // successful search

    eld::detail::pod_element_type<0, abc, TupleFeedAbc>::found_types f{};

    using found_int_t = eld::pod_element_t<0, abc, TupleFeedAbc>;
    static_assert(std::is_same<found_int_t, int>(), "");

    static_assert(std::is_same<int, eld::pod_element_t<0, abc, std::tuple<int>>>::value,
                  "");
    static_assert(std::is_same<int, eld::pod_element_t<0, abc, std::tuple<double, int>>>::value,
                  "");

    static_assert(std::is_same<std::string,
            eld::pod_element_t<6, abc, TupleFeedAbc>>::value, "Failed to deduce std::string in abc");

    constexpr auto offsetsAbc = eld::detail::get_pod_offsets<abc, TupleFeedAbc>();
    constexpr auto offsetA = offsetsAbc[0];
    static_assert(offsetA == offsetof(abc, a), "Invalid offset for int a");

    constexpr eld::detail::const_array<int, 16> constArray{4, 8, 15, 16, 23, 42};
    constexpr auto elemV = constArray[3];

    constexpr auto offsetB = offsetsAbc[1];
    static_assert(offsetsAbc[1] == offsetof(abc, b), "Invalid offset for float b");

    static_assert(eld::detail::pod_elem_offset<0, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, a), "Invalid offset for int a");
    static_assert(eld::detail::pod_elem_offset<1, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, b), "Invalid offset for float b");
    static_assert(eld::detail::pod_elem_offset<2, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, c), "Invalid offset for char c");
    static_assert(eld::detail::pod_elem_offset<3, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, d), "Invalid offset for char d");
    constexpr size_t offset_char_d = eld::detail::pod_elem_offset<3, abc, TupleFeedAbc>::value();
    size_t offset_int_e = eld::detail::pod_elem_offset<4, abc, TupleFeedAbc>::value();

    bool offset_remainder = !((offset_char_d + sizeof(char)) % alignof(abc));
    size_t align_abc = 4;//alignof(abc);
    size_t remainder = (offset_char_d + sizeof(char)) % align_abc;
    size_t remaining_bytes = align_abc - remainder;
    bool int_fits = remaining_bytes >= sizeof(int);
    size_t int_offset = offset_char_d + sizeof(char) +
            align_abc - (offset_char_d + sizeof(char)) % align_abc;

    static_assert(eld::detail::pod_elem_offset<4, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, e), "Invalid offset for int e");
    static_assert(eld::detail::pod_elem_offset<5, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, f), "Invalid offset for double f");
    static_assert(eld::detail::pod_elem_offset<6, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, g), "Invalid offset for std::string g");

    static_assert(eld::detail::pod_elem_offset<1, packed, TupleFeedAbc>::value() ==
                  offsetof(packed, b), "Invalid offset for int b");

    struct not_packed
    {
        char a;
        int b;
    };
    static_assert(eld::detail::pod_elem_offset<1, not_packed, TupleFeedAbc>::value() ==
                  offsetof(not_packed, b), "Invalid offset for int b");

    eld::detail::pod_elem_offset<1, abc, TupleFeedAbc>::value();
    eld::detail::pod_elem_offset<2, abc, TupleFeedAbc>::value();
    size_t offset_double_f = offsetof(abc, f),
            offset_of_double_f_calc = eld::detail::pod_elem_offset<5, abc, TupleFeedAbc>::value(),
            offset_string_g = offsetof(abc, g),
            offset_of_string_g_calc = eld::detail::pod_elem_offset<6, abc, TupleFeedAbc>::value();

    abc abcTest{};
    abcTest.e = 16;
    eld::get<4, TupleFeedAbc>(abcTest) = 24;

    struct printable_pod
    {
        int a;
        float b;
        std::string d;
    };

    struct printing
    {
        void operator()(int i)
        {
            std::cout << "int = " << i << std::endl;
        }

        void operator()(float f)
        {
            std::cout << "float = " << f << std::endl;
        }

        void operator()(const std::string &str)
        {
            std::cout << "std::string = " << str << std::endl;
        }

    };

    printable_pod printablePod{4, 8.1516f, "23 42"};

    size_t count = eld::for_each<TupleFeedAbc>(printablePod, printing());


    sizeof(std::string);

    static_assert(sizeof(abc) == eld::detail::evaluated_pod_size<TupleFeedAbc, abc>(),
            "abc evaluated size invalid");
    static_assert(sizeof(packed) == eld::detail::evaluated_pod_size<TupleFeedAbc, packed>(),
                  "packed evaluated size invalid");

    // test tail, size must be 8
    struct dummy
    {
        int a;
        char c;
        std::string d;
    };

    static_assert(sizeof(dummy) == eld::detail::evaluated_pod_size<TupleFeedAbc, dummy>(),
            "");

    using tuple_from_dummy = eld::pod_to_tuple_t<dummy, TupleFeedAbc>;
    static_assert(std::is_same<std::tuple<int, char, std::string>,
            tuple_from_dummy>(), "pod_to_tuple_t failed");

    sizeof(dummy);

    size_t sizePrintablePod =
        eld::detail::evaluated_pod_size<TupleFeedAbc, printable_pod>(),
                sizeInvalidPod =
            eld::detail::evaluated_pod_size<TupleFeedAbc, invalid_pod>(),
                    sizeDummy = eld::detail::evaluated_pod_size<TupleFeedAbc, dummy>();

    return 0;
}
