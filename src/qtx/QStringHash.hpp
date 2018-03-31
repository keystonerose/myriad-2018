#ifndef QTX_STD_HASH_HPP
#define QTX_STD_HASH_HPP

#include "ksr/hash.hpp"

#include <QChar>
#include <QString>

#include <cstddef>
#include <functional>

namespace std {

    template <>
    struct hash<QString> {
        auto operator()(const QString& value) const noexcept -> std::size_t {

            const auto data = value.data();
            const auto size = value.size() * sizeof(*data);
            return ksr::fnv1a_hash(reinterpret_cast<const unsigned char*>(data), size);
        }
    };
}


#endif
