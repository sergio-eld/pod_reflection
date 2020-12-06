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
        struct implicitly_convertible
        {
            // Any must be default constructable?
            template<typename Any>
            constexpr operator Any() const noexcept;
        };

        template<size_t s>
        using implicitly_convertible_s = implicitly_convertible;

        /*!
 * \TODO: description
 * @tparam Allowed
 */
        template<typename Allowed>
        struct explicitly_convertible
        {
            template<typename Any>
            constexpr operator Any() = delete;

            constexpr operator Allowed() const noexcept;
        };


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

        template<typename POD, size_t N, typename = make_index_sequence<N>>
        struct is_aggregate_initializable_from_n_args;

        template<typename POD, size_t N, size_t ... Indx>
        struct is_aggregate_initializable_from_n_args<POD, N, index_sequence<Indx...>> :
                is_aggregate_initializable<POD, implicitly_convertible_s<Indx>...>
        {
        };

        template<typename POD, typename T, size_t PODMemberIndex,
                typename = void,
                typename = make_index_sequence<PODMemberIndex>>
        struct is_pod_member_initializable_from_t : std::false_type
        {
        };

        template<typename POD, typename T, size_t PODMemberIndex,
                size_t ... PrevArgs>
        struct is_pod_member_initializable_from_t<POD,
                T,
                PODMemberIndex,
                void_t<decltype(POD{implicitly_convertible_s<PrevArgs>()..., explicitly_convertible<T>()})>,
                index_sequence<PrevArgs...>> : std::true_type
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

//    template<typename POD, size_t MemberIndex,
//            typename Tuple,
//            typename = detail::make_index_sequence<MemberIndex>>
//    class get_pod_member_type;
//
//    template<typename POD,
//            typename Tuple,
//            // typename ... FeedArgs,
//            size_t MemberIndex,
//            size_t ... Indx>
//    class get_pod_member_type<POD,
//            MemberIndex,
//            Tuple,
//            // std::tuple<FeedArgs...>,
//            detail::index_sequence<Indx...>>
//    {
//    public:
//
//    private:
//
//        template <size_t> struct tag_s{};
//
//        // out-of-tuple-range stop case method
//        constexpr static size_t find_type(tag_s<std::tuple_size<Tuple>::value>,
//                std::false_type)
//        {
//            return size_t() - 1;
//        }
//
//        // stop case method
//        template <size_t IndexFound>
//        constexpr static size_t find_type(tag_s<IndexFound>, std::true_type)
//        {
//            return IndexFound;
//        }
//
//        // recursive method
//        template <size_t I>
//        constexpr static size_t find_type(tag_s<I>, std::false_type)
//        {
//            using TupleTypeI = typename std::tuple_element<I, Tuple>::type;
//            return find_type(tag_s<I + 1>(),
//                    std::integral_constant<bool, std::is_same<>>)
//        }
//    };
}
