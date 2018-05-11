#ifndef QTX_EMISSION_FILTER_HPP
#define QTX_EMISSION_FILTER_HPP

#include "ksr/mem_fn_traits.hpp"

#include <QObject>

#include <cassert>
#include <chrono>
#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace qtx {

    /// Signal suppression modes for `EmissionFilter`. Each describes different criteria by which Qt
    /// signal emissions are skipped.

    enum class EmissionType {

        /// Performs an emission iff the arguments passed to the signal are different to the
        /// arguments that were passed the previous time an emission was attempted.

        Flush,

        /// Performs an emission iff the arguments passed to the signal are different to the
        /// arguments that were passed the previous time an emission was attempted **and** a
        /// sufficiently long interval of time has passed since the last emission that was actually
        /// performed.

        Timed,
    };

    /// Wrapper around the Qt `Q_EMIT` operation that selectively suppresses emissions in situations
    /// where a great many emissions would otherwise be performed in a short time period. The
    /// `EmissionType` enumeration describes the different modes of suppression that are available.
    ///
    /// `Target` must be a `QObject` subclass providing a signal to filter emissions of.
    /// `SignalArgs` must be the fully-qualified argument types for that signal, each of which must
    /// be copyable. (Non-copyable argument types are unable to be stored for comparison against the
    /// next group of arguments passed to the signal, and so are inconsistent with the operation of
    /// `EmissionFilter`.)

    template<typename SignalPtr>
    class EmissionFilter {
    public:

        using DecayArgs = typename ksr::mem_fn_traits<SignalPtr>::decay_args;
        using Target    = typename ksr::mem_fn_traits<SignalPtr>::cv_object;

        static constexpr auto DefaultInterval = std::chrono::milliseconds{50};

        /// Constructs a `EmissionFilter` object to suppress surplus emissions of the signal `sig`
        /// for the object `target`. The `target` reference must remain valid for the lifetime of
        /// the `EmissionFilter`. `interval` governs the length of the pause introduced by the
        /// `EmissionType::Timed` mode.

        explicit EmissionFilter(
            Target& target, SignalPtr sig, std::chrono::milliseconds interval = DefaultInterval)
          : _target{target}, _sig{sig}, _interval{interval} {
            assert(sig);
        }

        /// Performs an emission on the signal and target object that the `EmissionFilter` was
        /// constructed with, with the arguments `args`, iff the criteria associated with `type` are
        /// met. (See `EmissionType`.) Returns `true` if an emission was performed or `false` if it
        /// was suppressed.

        template<typename... Args>
        auto tryEmit(EmissionType type, Args&&... args) const -> bool {

            switch (type) {
            case EmissionType::Flush: return flushEmit(std::forward<Args>(args)...);
            case EmissionType::Timed: return timedEmit(std::forward<Args>(args)...);
            }

            assert(false);
            return false;
        }

    private:

        struct EmissionInfo {
            DecayArgs args;
            std::chrono::steady_clock::time_point time;
        };

        template<typename... Args>
        auto flushEmit(Args&&... args) const -> bool {

            if (_last && (std::tie(args...) == _last->args)) {
                return false;
            }

            _last = EmissionInfo{
                std::make_tuple(args...),
                std::chrono::steady_clock::now()};

            Target& target = _target.get();
            Q_EMIT (target.*_sig)(std::forward<Args>(args)...);
            return true;
        }

        template<typename... Args>
        auto timedEmit(Args&&... args) const -> bool {

            const auto now     = std::chrono::steady_clock::now();
            const auto elapsed = now - _last->time;

            return (elapsed >= _interval)
                && flushEmit(std::forward<Args>(args)...);
        }

        std::reference_wrapper<Target> _target;
        SignalPtr _sig;
        std::chrono::milliseconds _interval;

        mutable std::optional<EmissionInfo> _last;
    };
}

#endif
