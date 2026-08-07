--!editor
local kSpeed = 4.0
local kSens = 0.5

function on_ready()
    Camera.set_position(0.0, 0.0, 3.0)
end

function update(dt)
    if Input.is_pressed("rotate_focus") then
        Input.capture_mouse(true)
    elseif Input.is_released("rotate_focus") then
        Input.capture_mouse(false)
    end

    if Input.is_active("rotate_focus") then
        local dx = Input.axis("look_x")
        local dy = Input.axis("look_y")
        Camera.set_yaw(Camera.get_yaw() + dx * kSens)
        Camera.set_pitch(math.max(-89.9, math.min(89.9, Camera.get_pitch() - dy * kSens)))
    end

    local x, y, z = Camera.get_position()
    local yaw = math.rad(Camera.get_yaw())

    local forwardX = math.cos(yaw)
    local forwardZ = math.sin(yaw)
    local rightX = -math.sin(yaw)
    local rightZ = math.cos(yaw)

    local speed = kSpeed * dt

    if Input.is_active("move_forward") then
        Camera.set_position(x + forwardX * speed, y, z + forwardZ * speed)
    elseif Input.is_active("move_back") then
        Camera.set_position(x - forwardX * speed, y, z - forwardZ * speed)
    elseif Input.is_active("move_left") then
        Camera.set_position(x - rightX * speed, y, z - rightZ * speed)
    elseif Input.is_active("move_right") then
        Camera.set_position(x + rightX * speed, y, z + rightZ * speed)
    elseif Input.is_active("move_up") then
        Camera.set_position(x, y + speed, z)
    elseif Input.is_active("move_down") then
        Camera.set_position(x, y - speed, z)
    end
end
