# Default script structure

Before the update loop starts, the `on_ready` function will be called:
```lua
function on_ready()
    print("Ready function called!")
end
```

To receive update each frame, use `update(dt)` function:
```lua
function update(dt)
    print("Updating each frame!")
end
```

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
The `Camera` table controls the editor camera, which is not the scene camera. It is only available to editor scripts.
- `Camera.set_position(x, y, z)` moves the editor camera to the given position
    Example:

    ```lua
    Camera.set_position(10.0, 5.0, 0.0)
    ```
- `Camera.get_position()` returns the current editor camera position
    Example:

    ```lua
    local x, y, z = Camera.get_position()
    print("Camera at: " .. x .. ", " .. y .. ", " .. z)
    ```
- `Camera.set_yaw(yaw)` sets the camera rotation around the vertical axis (degrees)
- `Camera.get_yaw()` returns the current yaw (degrees)
- `Camera.set_pitch(pitch)` sets the camera vertical rotation (degrees)
- `Camera.get_pitch()` returns the current pitch (degrees)

`rotate_focus` is the right mouse button. While held, the cursor is captured and you can
rotate the camera by reading `look_x`/`look_y`. Example free-look:

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