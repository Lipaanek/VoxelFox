local self = setmetatable({}, Node)
local ticks = 10
local subscription

function ready()
    self.set_position(Vector3.new(4, 5, 6))
    subscription = EventBus.subscribe("test.ping", function(event)
        EventBus.emit("test.pong", {
            who = "b",
            value = event.value,
            ticks = ticks
        })
        EventBus.unsubscribe(subscription)
    end)
    EventBus.emit("test.ready", { who = "b" })
end

function update(dt)
    ticks = ticks + dt
    EventBus.emit("test.tick", { who = "b", ticks = ticks })
end
