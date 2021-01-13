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

struct MsgHeader
{
    uint32_t synchro;
//    TransceiverType recipientID: 4;
//    TransceiverType transmitterID: 4;
//    MessageID msgId;
    uint8_t reserved;
    uint32_t dateSec;
    uint32_t dateMicro;
    uint8_t msgCounter;
    uint32_t msgLength;
    uint16_t checksum;
//    MessageType msgType;
};

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

    static_assert(std::is_same<std::string,
            eld::pod_element_t<6, abc, TupleFeedAbc>>::value, "Failed to deduce std::string in abc");

    static_assert(eld::detail::pod_elem_offset<0, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, a), "Invalid offset for int a");
    static_assert(eld::detail::pod_elem_offset<1, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, b), "Invalid offset for float b");
    static_assert(eld::detail::pod_elem_offset<2, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, c), "Invalid offset for char c");
    static_assert(eld::detail::pod_elem_offset<3, abc, TupleFeedAbc>::value() ==
                  offsetof(abc, d), "Invalid offset for char d");
    size_t offset_char_d = eld::detail::pod_elem_offset<3, abc, TupleFeedAbc>::value();
    size_t offset_int_e = eld::detail::pod_elem_offset<4, abc, TupleFeedAbc>::value();

    bool offset_remainder = !((offset_char_d + sizeof(char)) % alignof(abc));
    size_t align_abc = 4;//alignof(abc);
    size_t remainder = (offset_char_d + sizeof(char)) % align_abc;
    size_t remaining_bytes = align_abc - remainder;
    bool int_fits = remaining_bytes >= sizeof(int);
    size_t int_offset = offset_char_d + sizeof(char) +
            align_abc - (offset_char_d + sizeof(char)) % align_abc;

//    static_assert(eld::detail::pod_elem_offset<4, abc, TupleFeedAbc>::value() ==
//                  offsetof(abc, e), "Invalid offset for int e");
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

    constexpr int folded = eld::detail::fold(1, 1, 1, 1);
    static_assert(folded == 4, "Failed to fold expressions");

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

    printable_pod printablePod{4, 8.1516, "23 42"};

    int count = eld::for_each<TupleFeedAbc>(printablePod, printing());

    eld::pod_size<MsgHeader>();
    static_assert(std::is_same<uint32_t,
            eld::pod_element_t<0, MsgHeader, eld::basic_feed>>::value, "");

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

    sizeof(dummy);

    size_t sizePrintablePod =
        eld::detail::evaluated_pod_size<TupleFeedAbc, printable_pod>(),
                sizeInvalidPod =
            eld::detail::evaluated_pod_size<TupleFeedAbc, invalid_pod>(),
                    sizeDummy = eld::detail::evaluated_pod_size<TupleFeedAbc, dummy>();



    // static_assert(eld::is_valid_pod<TupleFeedAbc, invalid_pod>(), "");

    return 0;
}
