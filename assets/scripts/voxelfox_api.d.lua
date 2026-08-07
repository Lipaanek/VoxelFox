---@meta
---Engine entry point, called once after the script is loaded
function on_ready() end

---Engine entry point, called every frame with delta time
---@param dt number
function update(dt) end

---@class Camera
Camera = {}

---@param x number
---@param y number
---@param z number
function Camera.set_position(x, y, z) end

---@return number, number, number
function Camera.get_position() end

---@param yaw number
function Camera.set_yaw(yaw) end

---@param pitch number
function Camera.set_pitch(pitch) end

---@return number
function Camera.get_yaw() end

---@return number
function Camera.get_pitch() end

---@class Input
Input = {}

---@param name string action name
---@return boolean
function Input.is_active(name) end

---@param name string action name
---@return boolean
function Input.is_pressed(name) end

---@param name string action name
---@return boolean
function Input.is_released(name) end

---@param name string axis name
---@return number
function Input.axis(name) end

---@param captured boolean
function Input.capture_mouse(captured) end
