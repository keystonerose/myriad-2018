#ifndef QTX_SIGNAL_FILTER_HPP
#define QTX_SIGNAL_FILTER_HPP

#include <QObject>

#include <cassert>
#include <chrono>
#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace qtx {

    /// TODO:DOC args must all be copyable
    /// target must be non-const if the signal is

    template <typename Target, typename... SignalArgs>
    class SignalFilter {
    public:

        static constexpr auto DefaultInterval = std::chrono::milliseconds{20};

        using SignalPtr = void (Target::*)(SignalArgs...);
        explicit SignalFilter(
            Target& target, SignalPtr sig, std::chrono::milliseconds interval = DefaultInterval)
          : _target{target}, _sig{sig}, _interval{interval} {
            assert(sig);
        }

        template <typename... Args>
        void forceEmit(Args&&... args) const {

            _last = EmissionInfo{
                std::make_tuple(args...),
                std::chrono::steady_clock::now()};

            Target& target = _target.get();
            Q_EMIT (target.*_sig)(std::forward<Args>(args)...);
        }

        template <typename... Args>
        auto tryEmit(Args&&... args) const -> bool {

            if (_last) {

                const auto now     = std::chrono::steady_clock::now();
                const auto elapsed = now - _last->time;

                if ((elapsed >= _interval) && (std::tie(args...) != _last->args)) {
                    forceEmit(std::forward<Args>(args)...);
                    return true;
                }
            }
            else {

                forceEmit(std::forward<Args>(args)...);
                return true;
            }

            return false;
        }

    private:

        struct EmissionInfo {
            std::tuple<std::decay_t<SignalArgs>...> args;
            std::chrono::steady_clock::time_point time;
        };

        std::reference_wrapper<Target> _target;
        SignalPtr _sig;
        std::chrono::milliseconds _interval;

        mutable std::optional<EmissionInfo> _last;
    };
}

#endif
