local self = setmetatable({}, Node)
local ticks = 0
local subscription

function ready()
    self.set_position(Vector3.new(1, 2, 3))
    subscription = EventBus.subscribe("test.ping", function(event)
        EventBus.emit("test.pong", {
            who = "a",
            value = event.value,
            ticks = ticks
        })
        EventBus.unsubscribe(subscription)
    end)
    EventBus.emit("test.ready", { who = "a" })
end

function update(dt)
    ticks = ticks + dt
    EventBus.emit("test.tick", { who = "a", ticks = ticks })
end
