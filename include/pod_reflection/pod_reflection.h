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
            template<typename To, typename = typename
            std::enable_if<std::is_same<To, Allowed>::value>::type>
            constexpr operator To() const noexcept;

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

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

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

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

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

    // TODO: add pointers?
    using basic_feed = std::tuple<bool,
            char, unsigned char,
            short, unsigned short,
            int, unsigned int,
            long, unsigned long,
            long long, unsigned long long,
            float, double, long double, long double
    >;

    // TODO: filter duplicates
    template<typename ... ArgsT>
    using extend_feed = decltype(std::tuple_cat(basic_feed(),
                                                std::tuple<ArgsT>()...));


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

    namespace detail
    {

        template<typename POD, typename TupleFeed = basic_feed>
        constexpr size_t pod_packing()
        {
            return sizeof(POD) % 4 ?
                   sizeof(POD) % 4 :
                   4;//alignof(POD);
        }

        // TODO: check this function!
        template<size_t I, typename POD, typename TupleFeed>
        class pod_elem_offset
        {
            static_assert(!std::is_same<undeduced, pod_element_t<I, POD, TupleFeed>>::value,
                          "Can't get an offset for an undeduced POD element!");

            template<size_t Indx>
            constexpr static size_t pod_elem_size()
            {
                return sizeof(pod_element_t<Indx, POD, TupleFeed>);
            }

        public:
            constexpr static std::ptrdiff_t packing = pod_packing<POD, TupleFeed>();

            constexpr static std::ptrdiff_t value()
            {
                return get_value(tag_s<0>(), 0);
            }

        private:
            // stop case
            constexpr static std::ptrdiff_t get_value(tag_s<I>, size_t offset)
            {
                return offset;
            }

            // general recursion
            template<size_t N>
            constexpr static std::ptrdiff_t get_value(tag_s<N>, size_t offset)
            {
                static_assert(!std::is_same<undeduced, pod_element_t<N, POD, TupleFeed>>::value,
                              "Can't get an offset for a POD element: failed to deduce one of POD elements' type!");

                // TODO: implement with packing
                return get_value(tag_s<N + 1>(),
                                 !((offset + pod_elem_size<N>()) % packing) ||
                                 packing - (offset + pod_elem_size<N>()) % packing >= pod_elem_size<N + 1>() ?
                                 offset + pod_elem_size<N>() :
                                 offset + pod_elem_size<N>() +
                                 packing - (offset + pod_elem_size<N>()) % packing
                );
            }

        };

        /*!
         * \warning Invalid implementation: alignof does not yield packing size of a struct
         * @tparam TupleFeed
         * @tparam POD
         * @return
         */
        template<typename TupleFeed, typename POD>
        constexpr size_t evaluated_pod_size()
        {
            return (pod_elem_offset<pod_size<POD>() - 1, POD, TupleFeed>::value() +
                    sizeof(pod_element_t<pod_size<POD>() - 1, POD, TupleFeed>)) % pod_packing<POD, TupleFeed>() ?
                   pod_elem_offset<pod_size<POD>() - 1, POD, TupleFeed>::value() +
                   sizeof(pod_element_t<pod_size<POD>() - 1, POD, TupleFeed>) +
                   pod_packing<POD, TupleFeed>() - (pod_elem_offset<pod_size<POD>() - 1, POD, TupleFeed>::value() +
                                                    sizeof(pod_element_t<pod_size<POD>() - 1, POD, TupleFeed>)) %
                                                   pod_packing<POD, TupleFeed>() :
                   pod_elem_offset<pod_size<POD>() - 1, POD, TupleFeed>::value() +
                   sizeof(pod_element_t<pod_size<POD>() - 1, POD, TupleFeed>);
        }

    }

    /*!
     * Checks if POD type is valid for accessing and manipulating elements.
     * \warning POD is not valid if it contains bitfields.
     * @tparam TupleFeed
     * @tparam POD
     * @return
     * \todo check this function
     */
    template<typename TupleFeed, typename POD>
    constexpr bool is_valid_pod()
    {
        // calculate expected POD size using offset
        // return sizeof(POD) == detail::evaluated_pod_size<TupleFeed, POD>();
        return true;
    }

    /*!
     * Extracts the Ith element from the POD structure
     * @tparam I - index of an element to access
     * @tparam TupleFeed - Tuple of types to be used as a feed to deduce POD elements
     * @tparam POD
     * @param pod
     * @return
     * \warning This function is UB. Need to write a workaround, i.e. - disable optimization
     */
    template<size_t I, typename TupleFeed, typename POD>
    pod_element_t<I, POD, TupleFeed> &get(POD &pod)
    {
        static_assert(!std::is_same<undeduced, pod_element_t<I, POD, TupleFeed>>::value,
                      "Can't get an undeduced POD element!");
        return *reinterpret_cast<pod_element_t<I, POD, TupleFeed> *>(((std::ptrdiff_t) &pod +
                                                                      detail::pod_elem_offset<I, POD, TupleFeed>::value()));
    }

    /*!
     * Extracts the Ith element from the POD structure
     * @tparam I - index of an element to access
     * @tparam TupleFeed - Tuple of types to be used as a feed to deduce POD elements
     * @tparam POD
     * @param pod
     * @return
     * \warning This function is UB. Need to write a workaround, i.e. - disable optimization
     */
    template<size_t I, typename TupleFeed, typename POD>
    const pod_element_t<I, POD, TupleFeed> &get(const POD &pod)
    {
        static_assert(!std::is_same<undeduced, pod_element_t<I, POD, TupleFeed>>::value,
                      "Can't get an undeduced POD element!");
        return *reinterpret_cast<const pod_element_t<I, POD, TupleFeed> *>(((std::ptrdiff_t) &pod +
                                                                            detail::pod_elem_offset<I, POD, TupleFeed>::value()));
    }

    namespace detail
    {
        constexpr int fold(int f)
        {
            return f;
        }

        // TODO: fix reverse order
        template<typename First, typename ... Last>
        constexpr int fold(First first, Last ... last)
        {
            return first + fold(last...);
        }

        template<typename POD, typename TupleFeed, typename = make_index_sequence<(pod_size<POD>())>>
        struct for_each_;

        template<typename POD, typename TupleFeed, size_t ... I>
        struct for_each_<POD, TupleFeed, index_sequence<I...>>
        {
            template<typename E, typename F>
            int invoke(E &pod_elem, F &f)
            {
                f(pod_elem);
                return 1;
            }

            // TODO: use recursion
            template<typename F>
            int operator()(POD &pod, F &&f)
            {
                auto func = std::forward<F>(f);
                return fold(invoke(get<I, TupleFeed>(pod), func)...);
            }
        };

        // TODO: for_each deduced, that is skipping undeduced elements
    }

    /*!
     * Invokes functor of type F for each element in a given POD. TupleFeed must be
     * provided for elements deduction
     * @tparam TupleFeed
     * @tparam POD
     * @tparam F
     * @param pod
     * @param func
     * @return
     * \warning Invokes elements in reverse order
     * \todo fix the order of elements invocation
     * \todo assert that a POD does not have bitfields
     */
    template<typename TupleFeed, typename POD, typename F>
    int for_each(POD &pod, F &&func)
    {
        static_assert(is_valid_pod<TupleFeed, POD>(),
                      "POD type is invalid: possibly contains bitfields");
        detail::for_each_<POD, TupleFeed> forEach{};
        return forEach(pod, std::forward<F>(func));
    }

}
