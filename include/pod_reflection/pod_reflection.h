#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>

namespace eld
{
    namespace detail
    {
        template<typename... Ts> struct make_void
        {
            typedef void type;
        };
        template<typename... Ts> using void_t = typename make_void<Ts...>::type;

        /*!
         * \TODO: description
         */
        // template<size_t>
        struct convertible_to_any
        {
            // Any must be default constructable
            template<typename Any>
            constexpr operator Any() const noexcept;
//            {
//                return Any();
//            }
        };

        template<size_t s>
        using convertible_to_any_s = convertible_to_any;

        // index sequence only
        template<size_t ...>
        struct index_sequence
        {
        };

        template<size_t N, size_t ... Next>
        struct index_sequence_helper : public index_sequence_helper<N - 1U, N - 1U, Next...>
        {
        };

        template<size_t ... Next>
        struct index_sequence_helper<0U, Next ...>
        {
            using type = index_sequence<Next ...>;
        };

        template<size_t N>
        using make_index_sequence = typename index_sequence_helper<N>::type;

        /*!
         * \TODO: description
         * @tparam Allowed
         */
        template<typename Allowed>
        struct convertible_to_one
        {
            template<typename Any>
            constexpr operator Any() = delete;

            constexpr operator Allowed() const noexcept;
        };

        /*!
         * \TODO: description
         * @tparam POD
         * @tparam Tuple
         */
        template<typename POD, typename Tuple, typename = void_t<>>
        struct is_aggregate_initializable_from_tuple : std::false_type
        {
        };

        /*!
         * \TODO: description
         * @tparam POD
         * @tparam Types
         */
        template<typename POD, typename ... Types>
        struct is_aggregate_initializable_from_tuple<POD,
                std::tuple<Types...>,
                void_t<decltype(POD{std::declval<Types>()...})>>
                : std::true_type
        {
        };

        template<typename Tuple, typename Add>
        using tuple_push_back = decltype(std::tuple_cat(Tuple(), std::tuple<Add>()));

        /*!
         * \TODO: description
         * @tparam T
         * @tparam From
         */
        template<typename T, typename ... From>
        struct is_aggregate_initializable : is_aggregate_initializable_from_tuple<T, std::tuple<From...>>
        {
            constexpr static size_t args_count = sizeof...(From);
        };

        template<typename POD, typename Tuple,
                bool = is_aggregate_initializable_from_tuple<POD, Tuple>()>
        struct count_args : std::integral_constant<size_t, std::tuple_size<Tuple>::value - 1>
        {
        };


        template<typename POD, size_t N, typename = make_index_sequence<N>>
        struct is_aggregate_initializable_from_n_args;

        template<typename POD, size_t N, size_t ... Indx>
        struct is_aggregate_initializable_from_n_args<POD, N, index_sequence<Indx...>> :
                is_aggregate_initializable<POD, convertible_to_any_s<Indx>...>
        {
        };


    }


    /*!
     * \TODO: description
     * @tparam POD
     */
    template<typename POD>
    class aggregate_args_counter
    {
    public:
        constexpr static size_t value()
        {
            using namespace detail;
            return sum_increment(tag_s<0>(), std::integral_constant<bool,
                    (bool) is_aggregate_initializable_from_n_args<POD, 0>()>());
        }

        constexpr operator size_t() const
        {
            return value();
        }

    private:
        template<size_t> struct tag_s
        {
        };

        template<size_t i>
        constexpr static size_t sum_increment(tag_s<i>, std::false_type)
        {
            return i - 1;
        }

        template<size_t i>
        constexpr static size_t sum_increment(tag_s<i>, std::true_type)
        {
            using namespace detail;
            return sum_increment(tag_s<i + 1>(),
                                 std::integral_constant<bool,
                                         (bool) is_aggregate_initializable_from_n_args<POD, i + 1>()>());
        }
    };


}
