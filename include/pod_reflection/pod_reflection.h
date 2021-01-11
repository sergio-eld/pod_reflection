#pragma once

#include <cstddef>
#include <limits>
#include <tuple>
#include <type_traits>

namespace eld
{
    namespace detail
    {

        template<size_t>
        struct tag_s
        {
        };

        template<typename... Ts> struct make_void
        {
            typedef void type;
        };
        template<typename... Ts> using void_t = typename make_void<Ts...>::type;

        /*!
         * \TODO: description
         */
        struct implicitly_convertible
        {
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

        template<typename, typename T, typename ...>
        struct is_aggregate_initialisable_ : std::false_type
        {
        };

        template<typename T, typename ... From>
        struct is_aggregate_initialisable_<
                void_t<decltype(T{std::declval<From>()...})>,
                T,
                From...> : std::true_type
        {
        };

        template<typename T, typename ... From>
        using is_aggregate_initialisable = is_aggregate_initialisable_<void_t<>, T, From...>;

        // TODO: remove?
        template<typename POD, size_t N, typename = make_index_sequence<N>>
        struct is_aggregate_initialisable_from_n_args;

        // TODO: remove?
        template<typename POD, size_t N, size_t ... Indx>
        struct is_aggregate_initialisable_from_n_args<POD, N, index_sequence<Indx...>> :
                is_aggregate_initialisable<POD, implicitly_convertible_s<Indx>...>
        {
        };

        template<typename POD, typename T, size_t PODMemberIndex,
                typename = void,
                typename = make_index_sequence<PODMemberIndex>>
        struct is_pod_member_initialisable_from_t : std::false_type
        {
        };

        template<typename POD, typename T, size_t PODMemberIndex,
                size_t ... PrevArgs>
        struct is_pod_member_initialisable_from_t<POD,
                T,
                PODMemberIndex,
                void_t<decltype(POD{implicitly_convertible_s<PrevArgs>()..., explicitly_convertible<T>()})>,
                index_sequence<PrevArgs...>> : std::true_type
        {
        };

        /*!
     * Helper class to count maximum arguments for aggregate initialization of a POD type
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
                        (bool) is_aggregate_initialisable_from_n_args<POD, 0>()>());
            }

        private:

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
                                             (bool) is_aggregate_initialisable_from_n_args<POD, i + 1>()>());
            }
        };

        template<size_t I, typename POD, typename TupleFeed>
        class tuple_index_from_pod_member
        {
            static_assert(std::tuple_size<TupleFeed>(), "TupleFeed must not be empty!");

        public:

            // TODO: remove
            template<typename T>
            using is_initializable = std::integral_constant<bool,
                    (bool) detail::is_pod_member_initialisable_from_t<POD, T, I>()>;

            constexpr static size_t value()
            {
                return find_type(detail::tag_s<0>(),
                                 is_initializable<typename std::tuple_element<0, TupleFeed>::type>());
            }

        private:

            // stop case on failure when tuple index exceeded
            constexpr static size_t find_type(detail::tag_s<std::tuple_size<TupleFeed>::value - 1>,
                                              std::false_type)
            {
                return size_t() - 1;
            }

            // stop case on success
            template<size_t tupleIndx>
            constexpr static size_t find_type(detail::tag_s<tupleIndx>, std::true_type)
            {
                return tupleIndx;
            }

            // general recursion
            template<size_t TupleIndex>
            constexpr static size_t find_type(detail::tag_s<TupleIndex>, std::false_type)
            {
                return find_type(detail::tag_s<TupleIndex + 1>(),
                                 is_initializable<typename std::tuple_element<TupleIndex + 1, TupleFeed>::type>());
            }
        };

    }


    // PUBLIC classes
    /*!
     * Provides access to the number of elements in a POD type as a compile-time constant expression
     * @tparam POD
     */
    template<typename POD>
    struct pod_size : public std::integral_constant<size_t, detail::aggregate_args_counter<POD>::value()>
    {
    };

    /*!
     * Tag for a type that couldn't be deduced from provided tuple feed
     */
    struct undeduced;

    namespace detail
    {
        template<size_t I, typename TupleFeed, bool = I >= std::tuple_size<TupleFeed>::value>
        struct deduced_tuple_elem
        {
            using type = undeduced;
        };

        template<size_t I, typename TupleFeed>
        struct deduced_tuple_elem<I, TupleFeed, false>
        {
            using type = typename std::tuple_element<I, TupleFeed>::type;
        };
    }

    /*!
     * Provides compile-time indexed access to the types of the elements of the POD type deduced from
     * the provided TupleFeed class
     * @tparam I - index of the element
     * @tparam POD - POD type
     * @tparam TupleFeed - Tuple of types to be used as a feed to deduce an element
     */
    template<size_t I, typename POD, typename TupleFeed>
    struct pod_element
    {
        static_assert(std::tuple_size<TupleFeed>(), "TupleFeed must not be empty!");

        using type = typename
        detail::deduced_tuple_elem<detail::tuple_index_from_pod_member<I, POD, TupleFeed>::value(),
                TupleFeed>::type;
    };

    template<size_t I, typename POD, typename TupleFeed>
    using pod_element_t = typename pod_element<I, POD, TupleFeed>::type;

    /// TODO: get<indx, tuple_feed>(pod)

}
