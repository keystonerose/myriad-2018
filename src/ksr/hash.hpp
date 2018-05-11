#ifndef KSR_HASH_HPP
#define KSR_HASH_HPP

#include <cstddef>
#include <cstdint>

namespace ksr {

    namespace _impl_hash {

        template<std::size_t size>
        struct fnv1a_constants;

        template<>
        struct fnv1a_constants<sizeof(std::uint32_t)> {
            static constexpr auto offset_basis = std::uint32_t{2166136261u};
            static constexpr auto prime        = std::uint32_t{16777619u};
        };

        template<>
        struct fnv1a_constants<sizeof(std::uint64_t)> {
            static constexpr auto offset_basis = std::uint64_t{14695981039346656037ull};
            static constexpr auto prime        = std::uint64_t{1099511628211ull};
        };
    }

    // Computes a FNV-1a hash for the `size` bytes of data pointed to by `data`. Currently supports
    // platforms with 32- and 64-bit sizes of `std::size_t` (though this could be extended).
    //
    // FNV-1a is a fast non-cryptographic hash function used in some standard library
    // implementations of `std::hash`. See:
    // <https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function>

    inline auto fnv1a_hash(const unsigned char* data, std::size_t size) noexcept -> std::size_t {

        using constants = _impl_hash::fnv1a_constants<sizeof(std::size_t)>;

        auto result = std::size_t{constants::offset_basis};
        for (auto i = std::size_t{0}; i < size; ++i) {

            result ^= static_cast<size_t>(data[i]);
            result *= constants::prime;
        }

        return result;
    }
}

#endif
