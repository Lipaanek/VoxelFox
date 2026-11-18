#include "event_bus.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace Util {

EventBus::~EventBus() {
    while (!m_subscriptions_.empty())
        unsubscribe(m_subscriptions_.begin()->first);
}

SubscriptionId EventBus::subscribeImpl(
    std::string eventName,
    std::function<void(const Event&)> listener,
    const void* owner,
    Cleanup cleanup
) {
    if (eventName.empty())
        throw std::invalid_argument("Event name cannot be empty");
    if (!listener)
        throw std::invalid_argument("Event listener cannot be empty");
    if (m_nextId_ == std::numeric_limits<SubscriptionId>::max())
        throw std::overflow_error("Event subscription IDs exhausted");

    const SubscriptionId id = m_nextId_++;
    m_eventSubscriptions_[eventName].push_back(id);
    m_subscriptions_.emplace(
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
    const auto subscription = m_subscriptions_.find(id);
    if (subscription == m_subscriptions_.end())
        return false;

    const std::string eventName = subscription->second.eventName;
    Cleanup cleanup = std::move(subscription->second.cleanup);
    m_subscriptions_.erase(subscription);

    const auto event = m_eventSubscriptions_.find(eventName);
    if (event != m_eventSubscriptions_.end()) {
        auto& ids = event->second;
        ids.erase(std::remove(ids.begin(), ids.end(), id), ids.end());
        if (ids.empty())
            m_eventSubscriptions_.erase(event);
    }

    if (cleanup)
        cleanup();
    return true;
}

void EventBus::unsubscribeOwner(const void* owner) {
    if (!owner)
        return;

    std::vector<SubscriptionId> ownedSubscriptions;
    for (const auto& [id, subscription] : m_subscriptions_) {
        if (subscription.owner == owner)
            ownedSubscriptions.push_back(id);
    }

    for (const SubscriptionId id : ownedSubscriptions)
        unsubscribe(id);
}

void EventBus::emitEvent(const std::string_view eventName, const Event& event) const {
    const auto eventSubscriptions = m_eventSubscriptions_.find(std::string(eventName));
    if (eventSubscriptions == m_eventSubscriptions_.end())
        return;

    const std::vector<SubscriptionId> snapshot = eventSubscriptions->second;
    for (const SubscriptionId id : snapshot) {
        const auto subscription = m_subscriptions_.find(id);
        if (subscription == m_subscriptions_.end())
            continue;

        const auto listener = subscription->second.listener;
        listener(event);
    }
}

} // namespace Util
