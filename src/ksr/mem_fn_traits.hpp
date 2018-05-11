#ifndef KSR_MEM_FN_TRAITS_HPP
#define KSR_MEM_FN_TRAITS_HPP

#include <tuple>
#include <type_traits>

namespace ksr {

    template<typename mem_fn_ptr>
    struct mem_fn_traits;

    // [[[cog
    // import cog
    //
    // traits_defn = r'''
    // template<typename ret, typename t, typename... args>
    // struct mem_fn_traits<ret (t::*)(args...){0}> {{
    //
    //     using result     = ret;
    //     using cv_object  = t{0};
    //     using decay_args = std::tuple<std::decay_t<args>...>;
    // }};
    // '''
    //
    // const_quals    = ['', 'const']
    // volatile_quals = ['', 'volatile']
    //
    // quals = [' '.join([c, v]).strip()
    //     for c in const_quals
    //     for v in volatile_quals]
    //
    // for qual in quals:
    //     cog.outl(traits_defn.format(qual and (' ' + qual)))
    //
    // ]]]

    template<typename ret, typename t, typename... args>
    struct mem_fn_traits<ret (t::*)(args...)> {

        using result     = ret;
        using cv_object  = t;
        using decay_args = std::tuple<std::decay_t<args>...>;
    };


    template<typename ret, typename t, typename... args>
    struct mem_fn_traits<ret (t::*)(args...) volatile> {

        using result     = ret;
        using cv_object  = t volatile;
        using decay_args = std::tuple<std::decay_t<args>...>;
    };


    template<typename ret, typename t, typename... args>
    struct mem_fn_traits<ret (t::*)(args...) const> {

        using result     = ret;
        using cv_object  = t const;
        using decay_args = std::tuple<std::decay_t<args>...>;
    };


    template<typename ret, typename t, typename... args>
    struct mem_fn_traits<ret (t::*)(args...) const volatile> {

        using result     = ret;
        using cv_object  = t const volatile;
        using decay_args = std::tuple<std::decay_t<args>...>;
    };

    // [[[end]]]
}

#endif
