#ifndef KSR_DETECT_HPP
#define KSR_DETECT_HPP

#include <type_traits>

// Implementation of the detection idiom for SFINAE. This is a pure C++17 shim for the corresponding
// `std::experimental` facilities from the Library Fundamentals TS v2; refer to the documentation
// for that technical specification for full details. This implementation is based on the sample
// from [cppreference](http://en.cppreference.com/w/cpp/experimental/is_detected).

namespace ksr {

    namespace _impl_detect {

        template<
            typename default_type,
            typename always_void,
            template<typename...> typename op,
            typename... args>

        struct detector {
            using type    = default_type;
            using value_t = std::false_type;
        };

        template<
            typename default_type,
            template<typename...> typename op,
            typename... args>

        struct detector<default_type, std::void_t<op<args...>>, op, args...> {
            using type    = op<args...>;
            using value_t = std::true_type;
        };
    }

    struct nonesuch {

        nonesuch()  = delete;
        ~nonesuch() = delete;

        nonesuch(const nonesuch&)       = delete;
        void operator=(const nonesuch&) = delete;
    };

    template<template<typename...> typename op, typename... args>
    using is_detected = typename _impl_detect::detector<nonesuch, void, op, args...>::value_t;

    template<template<typename...> class op, class... args>
    using detected_t = typename _impl_detect::detector<nonesuch, void, op, args...>::type;

    template<class Default, template<typename...> class op, class... args>
    using detected_or = _impl_detect::detector<Default, void, op, args...>;
}

#endif

