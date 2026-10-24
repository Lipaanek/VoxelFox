# Node-attached scripts

Lua 5.4 does not support `extends` in a local declaration. A node-attached script
gets a `Node` base table whose methods are bound to that specific node. Use Lua's
standard metatable syntax to inherit those methods:

```lua
local self = setmetatable({}, Node)

function ready()
    local position = self.get_position()
    self.set_position(Vector3.new(position.x, position.y, position.z))
end

function update(dt)
    -- Called once per frame for this node.
end
```

Each script has its own Lua environment, so its `ready`/`update` callbacks and
global state do not replace those of other node scripts. Node methods use dot
calls (`self.set_position(...)`) and currently include `get_name`, `get_child`,
`get_node`, `get_root`, `get_position`, and `set_position`. `Node3D` positions
use `Vector3`; UI node positions use a table with `x` and `y` fields.

## Event bus

`EventBus.subscribe(name, callback)` subscribes to an event and returns a
subscription ID. `EventBus.emit(name, data)` emits a named event with an
optional table of scalar fields. Payload keys must be strings and values must
be booleans, numbers, or strings. The callback receives an event table with
the event `name` and all payload fields. Use `EventBus.unsubscribe(id)` to
remove a subscription.

```lua
local subscription

function ready()
    subscription = EventBus.subscribe("game.started", function(event)
        print("Received:", event.name, event.level)
    end)

    EventBus.emit("game.started", { level = "forest" })
end
```

Subscriptions are cleaned up when their script is replaced or its node is
removed. Engine events use typed C++ event subclasses; their scalar payload
fields are available to Lua subscribers where provided.

C++ event types derive from `Util::Event` and can carry strongly typed members.
Populate the inherited `data` map with scalar values when Lua subscribers also
need to see those fields:

```cpp
struct DoorOpenedEvent : Util::Event {
    int doorId;
};

scene.eventBus().subscribe<DoorOpenedEvent>(
    "door.opened",
    [](const DoorOpenedEvent& event) {
        // Use the typed event fields.
    }
);

DoorOpenedEvent event;
event.doorId = 12;
event.data.emplace("door_id", std::int64_t{12});
scene.eventBus().emit("door.opened", event);
```

# Default script structure

Before the update loop starts, the `ready` function will be called:
```lua
function ready()
    print("Ready function called!")
end
```

To receive update each frame, use `update(dt)` function:
```lua
function update(dt)
    print("Updating each frame!")
end
```

# Runtime UI (C++)

Runtime UI is scene-owned and built with the engine's C++ UI builders; it is
separate from the dockable ImGui panels used by editor tooling. UI elements are
screen-space widgets and do not create movable or dockable editor windows.

The logical canvas is 1080 units tall and scales uniformly with the viewport.
Top-level elements are anchored to the viewport; child positions are relative
to their parent container. Position values are offsets from the selected
anchor, with positive X moving right and positive Y moving down. Use negative
offsets with right/bottom anchors to inset a HUD element from an edge.

```cpp
scene.addUIElement(makeContainer(
    "hud",
    { 16.0f, 16.0f },
    { 320.0f, 144.0f },
    {
        makeText("title", "Game HUD", { 8.0f, 8.0f }),
        makeButton("action", "Use", [] {}, { 8.0f, 40.0f }, { 128.0f, 32.0f }),
        makeCheckbox("enabled", "Enabled", [] {}, false, { 8.0f, 88.0f }),
    }
));

scene.addUIElement(makeText(
    "hint",
    "Press Escape to pause",
    { -16.0f, -16.0f },
    UIAnchor::BottomRight
));
```

The current runtime widget set is containers, text, buttons, and checkboxes.
The engine routes UI input separately from gameplay actions while a widget is
being interacted with. Runtime UI currently has a C++ API; it is not exposed to
Lua scripts.

# Input functions
- `Input.is_active(action_name)` checks if the action name is active
    Example:

    ```lua
    if Input.is_active("move_forward") then
        print("Pressed forward!")
    end
    ```
- `Input.is_pressed(action_name)` checks if the action name is being pressed
    Example:

    ```lua
    if Input.is_pressed("move_forward") then
        print("Moving forward!")
    end
    ```
- `Input.is_released(action_name)` checks if the action name is released
    Example:

    ```lua
    if Input.is_released("move_forward") then
        print("Not moving forward anymore!")
    end
    ```
- `Input.axis(action_name)` returns the mouse/scroll movement for this frame
    - Returns 0 when the mouse hasn't moved / wheel isn't scrolled
    - "look_x" and "look_y" are default mouse-axis actions
    - "zoom" is scroll
    Example:

    ```lua
    local dx = Input.axis("look_x")
    local dy = Input.axis("look_y")

    if dx ~= 0 or dy ~= 0 then
        print("Mouse moved by: " .. dx .. ", " .. dy)
    end

    local zoom = Input.axis("zoom")
    if zoom ~= 0 then
        print("Scrolled: " .. zoom)
    end
    ```
- `Input.capture_mouse(true/false)` locks or releases the cursor
    - true -> cursor captured, raw mouse movement (mouse-look mode)
    - false -> cursor visible and free (for example used for UI)
    Example:
    
    ```lua
    -- Enter mouse-look mode
    Input.capture_mouse(true)

    -- Release the cursor when done (e. g. after clicking a button)
    Input.capture_mouse(false)
    ```

All the default actions are the following: `look_x`, `look_y`, `zoom`, `rotate_focus`, `move_forward`, `move_back`, `move_left`, `move_right`, `move_up`, `move_down` and `boost`

> [!NOTE]
> `is_pressed` and `is_released` fire on the frame of the transition, not continuously

# Camera functions
The `Camera` table controls the active environment camera used to render the scene. It is available to editor and scene/node scripts.
- `Camera.set_position(pos)` moves the camera to the given `Vector3`
    Example:

    ```lua
    Camera.set_position(Vector3.new(10.0, 5.0, 0.0))
    ```
- `Camera.get_position()` returns the current position as a `Vector3`
    Example:

    ```lua
    local pos = Camera.get_position()
    print("Camera at: " .. pos.x .. ", " .. pos.y .. ", " .. pos.z)
    ```
- `Camera.set_yaw(yaw)` sets the camera rotation around the vertical axis (degrees)
- `Camera.get_yaw()` returns the current yaw (degrees)
- `Camera.set_pitch(pitch)` sets the camera vertical rotation (degrees)
- `Camera.get_pitch()` returns the current pitch (degrees)

# Adding engine Lua APIs
Each `LuaEngine` receives the same environment API modules through `LuaApiRegistry`. Modules are installed before scripts run, including scene/node scripts, and each engine keeps its own Lua state.

To add an API in C++, implement a binding function with the `LuaApiRegistry::BindingModule` signature:

```cpp
void registerClockApi(lua_State* L, const LuaApiContext& context);
```

Register it in `LuaApiRegistry::createDefault()`. If the binding needs an engine service, add that non-owning service to `LuaApiContext` and validate it in the module before registering its Lua functions. This keeps binding setup out of script callers and makes the new API available to both environment and scene Lua states.

`rotate_focus` is the right mouse button. While held, the cursor is captured and you can rotate the camera by reading `look_x`/`look_y`. Example free-look:

```lua
function update(dt)
    if Input.is_pressed("rotate_focus") then
        Input.capture_mouse(true)
    elseif Input.is_released("rotate_focus") then
        Input.capture_mouse(false)
    end

    if Input.is_active("rotate_focus") then
        local dx = Input.axis("look_x")
        local dy = Input.axis("look_y")
        Camera.set_yaw(Camera.get_yaw() + dx * 0.1)
        Camera.set_pitch(math.max(-89.0, math.min(89.0, Camera.get_pitch() - dy * 0.1)))
    end
end
```