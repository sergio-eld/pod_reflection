#pragma once

/*This library implements reflection for POD data type structures

MIT License

Copyright (c) 2019 ElDesalmado

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _TUPLE_
#include <tuple>
#endif


typedef std::tuple<bool, 
    char, unsigned char, 
    short, unsigned short, 
    int, unsigned int, 
    long, unsigned long, 
    long long, unsigned long long, 
    float, double, long double, long double 
    /* std::string will not compile as not constexpr*/> base_types;

//define custom tuple to extend pod_map
#ifndef POD_EXTENDS
typedef base_types pod_map;
#else
typedef decltype(std::tuple_cat(POD_EXTENDS(), base_types())) pod_map;
#endif // !POD_EXTENDS

template< size_t indx, class ...>
struct cexpr_iter
{
	constexpr static size_t index = indx;
};

template <size_t iter, class tuple_list = pod_map>
class pod_iter;

template <size_t iter, class ... types>
class pod_iter<iter, std::tuple<types...>>
{
    //TODO: can substitute this with std??
	template <size_t it>
	constexpr static auto get_type()
	{
		if constexpr (iter >= sizeof...(types))
			return void();
		else return std::tuple_element_t<it, std::tuple<types...>>();
	}

public:
	using type = decltype(get_type<iter>());
};

//to generate index sequence
template <size_t sz>
using indx_seq = decltype(std::make_index_sequence<sz>());
/*
struct iseq_type
{
    using indx_seq = decltype(std::make_index_sequence<sz>()) ;
};
*/
template <class tuple1, class tuple2>
using tuple_concat = decltype(std::tuple_cat(tuple1(), tuple2()));

template <class pack /*tuple of consecutive class's fields' types*/>
struct mem_layout_info;

//helper class
template <class ... cl_args>
class mem_layout_info<std::tuple<cl_args...>>
{

    template <size_t ... sizes>
    constexpr static size_t get_sum()
    {
        return (sizes + ...);
    }

    template <size_t arg_num, size_t prev_pad = 0, size_t ... indx>
    constexpr static size_t get_padding(std::index_sequence<indx...>&&)
    {
        constexpr size_t cur_size = sizeof(std::tuple_element_t<sizeof...(indx), std::tuple<cl_args...>>);
        constexpr size_t cur_pad = (prev_pad % 4) >= cur_size ? prev_pad : prev_pad + (prev_pad % 4);
        if constexpr (sizeof...(indx) == arg_num)
            return cur_pad;
        else
            return get_padding<arg_num, cur_pad + cur_size>(indx_seq<sizeof...(indx) + 1>());
    }

public:

    template <size_t elem_num>
    using arg_type = typename std::tuple_element_t<elem_num, std::tuple<cl_args...>>;

    using fields_sizes = typename std::index_sequence<sizeof(cl_args)...>;

    template <size_t elem_num>
    constexpr static size_t padding = get_padding<elem_num>(indx_seq<0>());

    constexpr static size_t total_args_size = get_sum<sizeof(cl_args)...>(),
        class_size = padding<sizeof...(cl_args) - 1> + sizeof(arg_type< sizeof...(cl_args) - 1>); 

};


template <class POD, class types_map = pod_map, class divisor = char, size_t predict = sizeof(POD) / sizeof(divisor) + 1>
class refl_traits
{
    template <size_t I>
    struct ubiq_constructor
    {
        template <typename Other>
        constexpr operator Other&() const noexcept
        {
            return Other(*this);
        }
    };

    template <class allowed>
    struct ubiq_explicit
    {
        template <class other>
        constexpr operator other&() = delete;
        constexpr operator allowed&() noexcept;
    };

    template <class, class ... POD /*and index sequence*/>
    struct args_allowed_ : public std::false_type
    {};

    template <class POD, size_t ... indx>
    struct args_allowed_ < std::void_t<decltype(POD{ ubiq_constructor<indx>() ... }) > , POD, std::index_sequence<indx... >> : public std::true_type
    {};

    template <class POD, class T, size_t ... indx>
    struct args_allowed_ < std::void_t<decltype(POD{ ubiq_constructor<indx>() ..., ubiq_explicit<T>() }) > , POD, T, std::index_sequence<indx... >> : public std::true_type
    {};


    template <size_t map_iter = 0, class ... prev_args>
    constexpr static auto get_types()
    {
		if constexpr (std::is_same_v<pod_iter<map_iter>::type, void>)
			//static_assert(false, "Provided map could not deduce argument ¹");
			return cexpr_iter<sizeof...(prev_args)>();
		else
		{
			typedef pod_iter<map_iter, types_map>::type iter_type;

			if constexpr (sizeof...(prev_args) == fields_count())
				return std::tuple<prev_args...>();
			else if constexpr (is_valid_arg<iter_type, sizeof...(prev_args)>::value)
			    return get_types<0, prev_args..., iter_type>();
			else return get_types<map_iter + 1, prev_args...>();
		}

    }

public:
    template <size_t pred_start = predict>
    constexpr static size_t fields_count()
    {
//        static_assert(std::is_aggregate_v<POD>, "Provided class can not be aggregate initialized!");
        if constexpr (!args_allowed<pred_start>::value)
            return fields_count<pred_start - 1>();
        else return pred_start;
    }

    template <size_t predict_>
    using args_allowed = args_allowed_<std::void_t<>, POD, indx_seq<predict_>>;

    template <class T, size_t arg_num>
    using is_valid_arg = args_allowed_<std::void_t<>, POD, T, indx_seq<arg_num>>;

    using field_types = typename decltype(get_types<0>());	//vs 17 express does not allow default args

    template <size_t arg_num, class out = mem_layout_info<field_types>::arg_type<arg_num>>
    static const out& get(const POD& obj)
    {
        size_t addr = (size_t)&obj;
        return *(out*)(addr + mem_layout_info<field_types>::padding<arg_num>);
    }

};