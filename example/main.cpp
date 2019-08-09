#include <string>
#include <iostream>

#define POD_EXTENDS std::tuple<std::string>
#include "pod_reflection.hpp"

struct abc
{
    int a = 4;
    float b = 8.15;
    char c = 'c', d = 'd';
    int e = 16;
    double f = 23.42;
    std::string g = "oceanic";
};

int main(int argc, char** argv)
{
    //assert number of args allowed for aggregate initialization
    static_assert(!refl_traits<abc>::args_allowed<16>::value, "Wrong output!");
    static_assert(!refl_traits<abc>::args_allowed<8>::value, "Wrong output!");

    static_assert(refl_traits<abc>::args_allowed<6>::value, "Wrong output!");
    static_assert(refl_traits<abc>::args_allowed<5>::value, "Wrong output!");

    static_assert(refl_traits<abc>::is_valid_arg<int, 0>::value, "Wrong output!");
    static_assert(!refl_traits<abc>::is_valid_arg<int, 1>::value, "Wrong output!");

    static_assert(refl_traits<abc>::is_valid_arg<float, 1>::value, "Wrong output!");
    static_assert(!refl_traits<abc>::is_valid_arg<float, 2>::value, "Wrong output!");

    static_assert(refl_traits<abc>::is_valid_arg<char, 2>::value, "Wrong output!");
    static_assert(refl_traits<abc>::is_valid_arg<char, 3>::value, "Wrong output!");
    static_assert(!refl_traits<abc>::is_valid_arg<char, 4>::value, "Wrong output!");

    static_assert(refl_traits<abc>::is_valid_arg<int, 4>::value, "Wrong output!");
    static_assert(!refl_traits<abc>::is_valid_arg<int, 5>::value, "Wrong output!");

    static_assert(refl_traits<abc>::is_valid_arg<double, 5>::value, "Wrong output!");
    static_assert(!refl_traits<abc>::is_valid_arg<double, 6>::value, "Wrong output!");

    static_assert(refl_traits<abc>::is_valid_arg<std::string, 6>::value, "Wrong output!");
    static_assert(!refl_traits<abc>::is_valid_arg<std::string, 7>::value, "Wrong output!");

    static_assert(refl_traits<abc>::fields_count() == 7, "Wrong output!");
    static_assert(refl_traits<abc>::fields_count() != 6, "Wrong output!");

    typedef refl_traits<abc>::field_types abc_types;

    static_assert(std::is_same_v<abc_types, std::tuple<int, float, char, char, int, double, std::string>>, "Wrong output!");

    static_assert(std::is_same_v<mem_layout_info<abc_types>::fields_sizes,
        std::index_sequence<sizeof(abc::a), sizeof(abc::b), sizeof(abc::c), sizeof(abc::d), sizeof(abc::e), sizeof(abc::f), sizeof(abc::g)>>,
        "Wrong output!");

    mem_layout_info<abc_types>::total_args_size;
    static_assert(mem_layout_info<abc_types>::class_size == sizeof(abc), "Wrong output!");

    static_assert(mem_layout_info<abc_types>::padding<0> == offsetof(abc, a), "Wrong output!");
    static_assert(mem_layout_info<abc_types>::padding<1> == offsetof(abc, b), "Wrong output!");
    static_assert(mem_layout_info<abc_types>::padding<2> == offsetof(abc, c), "Wrong output!");
    static_assert(mem_layout_info<abc_types>::padding<3> == offsetof(abc, d), "Wrong output!");
    static_assert(mem_layout_info<abc_types>::padding<4> == offsetof(abc, e), "Wrong output!");
    static_assert(mem_layout_info<abc_types>::padding<5> == offsetof(abc, f), "Wrong output!");
    static_assert(mem_layout_info<abc_types>::padding<6> == offsetof(abc, g), "Wrong output!");

    abc abc_a;
    std::cout << refl_traits<abc>::get<0>(abc_a) << std::endl;
    std::cout << refl_traits<abc>::get<1>(abc_a) << std::endl;
    std::cout << refl_traits<abc>::get<2>(abc_a) << std::endl;
    std::cout << refl_traits<abc>::get<3>(abc_a) << std::endl;
    std::cout << refl_traits<abc>::get<4>(abc_a) << std::endl;
    std::cout << refl_traits<abc>::get<5>(abc_a) << std::endl;
    std::cout << refl_traits<abc>::get<6>(abc_a) << std::endl;

	return 0;
}