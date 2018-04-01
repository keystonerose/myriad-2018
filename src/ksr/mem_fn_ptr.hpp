#ifndef KSR_CV_LIKE_HPP
#define KSR_CV_LIKE_HPP

namespace ksr {

    /// Type transformation for forming pointer types to correctly cv-qualified member functions in
    /// cases where the cv-qualification of the `this` type for the member function indicates
    /// whether the pointer should be to a correspondingly `const`- or `volatile`- qualified member
    /// function.
    ///
    /// That is, if `t` is equivalent to `t0 [cv-qual]` for some cv-unqualified type `u` and
    /// cv-qualification `cv-qual`, then `mem_fn_ptr<t<ret, t, args...>` is an alias for
    /// `ret (t0::*)(args...) [cv-qual]`.

    template <typename ret, typename t, typename... args>
    struct mem_fn_ptr {
        using type = ret (t::*)(args...);
    };

    template <typename ret, typename t, typename... args>
    struct mem_fn_ptr<ret, const t, args...> {
        using type = ret (t::*)(args...) const;
    };

    template <typename ret, typename t, typename... args>
    struct mem_fn_ptr<ret, volatile t, args...> {
        using type = ret (t::*)(args...) volatile;
    };

    template <typename ret, typename t, typename... args>
    struct mem_fn_ptr<ret, const volatile t, args...> {
        using type = ret (t::*)(args...) const volatile;
    };

    template <typename ret, typename t, typename... args>
    using mem_fn_ptr_t = typename mem_fn_ptr<ret, t, args...>::type;
}

#endif

