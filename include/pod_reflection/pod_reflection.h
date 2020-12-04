#pragma once

#include <cstddef>
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
        template<size_t>
        struct convertible_to_any
        {
            // Any must be default constructable
            template<typename Any>
            constexpr operator Any() const noexcept;
//            {
//                return Any();
//            }
        };

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

        /*!
         * \TODO: description
         * @tparam T
         * @tparam From
         */
        template<typename T, typename ... From>
        struct is_aggregate_initializable : is_aggregate_initializable_from_tuple<T, std::tuple<From...>>
        {
        };

    }

}
