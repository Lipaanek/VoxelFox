#include "event_bus.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace Util {

EventBus::~EventBus() {
    while (!subscriptions_.empty())
        unsubscribe(subscriptions_.begin()->first);
}

EventBus::SubscriptionId EventBus::subscribeImpl(
    std::string eventName,
    std::function<void(const Event&)> listener,
    const void* owner,
    Cleanup cleanup
) {
    if (eventName.empty())
        throw std::invalid_argument("Event name cannot be empty");
    if (!listener)
        throw std::invalid_argument("Event listener cannot be empty");
    if (nextId_ == std::numeric_limits<SubscriptionId>::max())
        throw std::overflow_error("Event subscription IDs exhausted");

    const SubscriptionId id = nextId_++;
    eventSubscriptions_[eventName].push_back(id);
    subscriptions_.emplace(
        id,
        Subscription {
            .eventName = std::move(eventName),
            .listener = std::move(listener),
            .owner = owner,
            .cleanup = std::move(cleanup)
        }
    );
    return id;
}

bool EventBus::unsubscribe(const SubscriptionId id) {
    const auto subscription = subscriptions_.find(id);
    if (subscription == subscriptions_.end())
        return false;

    const std::string eventName = subscription->second.eventName;
    Cleanup cleanup = std::move(subscription->second.cleanup);
    subscriptions_.erase(subscription);

    const auto event = eventSubscriptions_.find(eventName);
    if (event != eventSubscriptions_.end()) {
        auto& ids = event->second;
        ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
        if (ids.empty())
            eventSubscriptions_.erase(event);
    }

    if (cleanup)
        cleanup();
    return true;
}

void EventBus::unsubscribeOwner(const void* owner) {
    if (!owner)
        return;

    std::vector<SubscriptionId> ownedSubscriptions;
    for (const auto& [id, subscription] : subscriptions_) {
        if (subscription.owner == owner)
            ownedSubscriptions.push_back(id);
    }

    for (const SubscriptionId id : ownedSubscriptions)
        unsubscribe(id);
}

void EventBus::emitEvent(const std::string_view eventName, const Event& event) const {
    const auto eventSubscriptions = eventSubscriptions_.find(std::string(eventName));
    if (eventSubscriptions == eventSubscriptions_.end())
        return;

    const std::vector<SubscriptionId> snapshot = eventSubscriptions->second;
    for (const SubscriptionId id : snapshot) {
        const auto subscription = subscriptions_.find(id);
        if (subscription == subscriptions_.end())
            continue;

        const auto listener = subscription->second.listener;
        listener(event);
    }
}

} // namespace Util
