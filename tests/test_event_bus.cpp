#include "catch_amalgamated.hpp"

#include "core\util\event_bus.hpp"

namespace {

struct NumberEvent final : Util::Event {
    explicit NumberEvent(const int value)
        : value(value) {
        data.emplace("value", static_cast<std::int64_t>(value));
    }

    int value;
};

struct TextEvent final : Util::Event {
};

}

TEST_CASE("EventBus dispatches named events to matching typed subscribers") {
    Util::EventBus eventBus;
    int numberCalls = 0;
    int baseCalls = 0;

    eventBus.subscribe<NumberEvent>(
        "test.value",
        [&numberCalls](const NumberEvent& event) {
            numberCalls += event.value;
        }
    );
    eventBus.subscribe<Util::Event>(
        "test.value",
        [&baseCalls](const Util::Event&) {
            ++baseCalls;
        }
    );
    eventBus.subscribe<TextEvent>("test.other", [](const TextEvent&) {});

    eventBus.emit("test.value", NumberEvent(42));
    REQUIRE(numberCalls == 42);
    REQUIRE(baseCalls == 1);
}

TEST_CASE("EventBus snapshots listeners and honors unsubscription during emission") {
    Util::EventBus eventBus;
    int calls = 0;
    Util::EventBus::SubscriptionId secondSubscription = 0;

    eventBus.subscribe<NumberEvent>(
        "test.snapshot",
        [&eventBus, &calls, &secondSubscription](const NumberEvent&) {
            ++calls;
            eventBus.unsubscribe(secondSubscription);
        }
    );
    secondSubscription = eventBus.subscribe<NumberEvent>(
        "test.snapshot",
        [&calls](const NumberEvent&) {
            calls += 10;
        }
    );

    eventBus.emit("test.snapshot", NumberEvent(1));
    REQUIRE(calls == 1);
}

TEST_CASE("EventBus cleans up subscriptions owned by a script") {
    Util::EventBus eventBus;
    int owner = 0;
    int calls = 0;
    int cleanups = 0;

    eventBus.subscribe<NumberEvent>(
        "test.owner",
        [&calls](const NumberEvent&) { ++calls; },
        &owner,
        [&cleanups] { ++cleanups; }
    );

    eventBus.unsubscribeOwner(&owner);
    eventBus.emit("test.owner", NumberEvent(1));

    REQUIRE(calls == 0);
    REQUIRE(cleanups == 1);
}
