#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "event.hpp"

namespace Util {

class EventBus {
public:
    using SubscriptionId = std::int64_t;
    using Cleanup = std::function<void()>;

    EventBus() = default;
    ~EventBus();

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    template <typename TEvent, typename TListener>
    SubscriptionId subscribe(
        std::string eventName,
        TListener&& listener,
        const void* owner = nullptr,
        Cleanup cleanup = {}
    ) {
        static_assert(std::is_base_of_v<Event, TEvent>);
        std::function<void(const TEvent&)> typedListener(std::forward<TListener>(listener));
        return subscribeImpl(
            std::move(eventName),
            [typedListener = std::move(typedListener)](const Event& event) {
                if (const auto* typedEvent = dynamic_cast<const TEvent*>(&event))
                    typedListener(*typedEvent);
            },
            owner,
            std::move(cleanup)
        );
    }

    template <typename TEvent>
    void emit(std::string_view eventName, const TEvent& event) const {
        static_assert(std::is_base_of_v<Event, TEvent>);
        emitEvent(eventName, event);
    }

    bool unsubscribe(SubscriptionId id);
    void unsubscribeOwner(const void* owner);

private:
    struct Subscription {
        std::string eventName;
        std::function<void(const Event&)> listener;
        const void* owner;
        Cleanup cleanup;
    };

    SubscriptionId nextId_ = 1;
    std::unordered_map<SubscriptionId, Subscription> subscriptions_;
    std::unordered_map<std::string, std::vector<SubscriptionId>> eventSubscriptions_;

    SubscriptionId subscribeImpl(
        std::string eventName,
        std::function<void(const Event&)> listener,
        const void* owner,
        Cleanup cleanup
    );
    void emitEvent(std::string_view eventName, const Event& event) const;
};

} // namespace Util
