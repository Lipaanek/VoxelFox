---@meta
---Engine entry point, called once after the script is loaded
function ready() end

---Engine entry point, called every frame with delta time
---@param dt number
function update(dt) end

---@class Vector3
---@field x number
---@field y number
---@field z number
Vector3 = {}

---@constructor
---@param x number
---@param y number
---@param z number
---@return Vector3
function Vector3.new(x, y, z) end

---@class Vector2
---@field x number
---@field y number
Vector2 = {}

---@constructor
---@param x number
---@param y number
---@return Vector2
function Vector2.new(x, y) end

---@class Node
Node = {}

---@return string
function Node.get_name() end

---@param name string
---@return Node?
function Node.get_child(name) end

---@param path string
---@return Node?
function Node.get_node(path) end

---@return Node?
function Node.get_root() end

---@param position Vector2|Vector3
function Node.set_position(position) end

---@return Vector2|Vector3
function Node.get_position() end

---@class Event
---@field name string

---@class EventBus
EventBus = {}

---@param name string
---@param callback fun(event: Event)
---@return integer subscriptionId
function EventBus.subscribe(name, callback) end

---@param name string
---@param data table<string, boolean|number|string>?
function EventBus.emit(name, data) end

---@param subscriptionId integer
---@return boolean unsubscribed
function EventBus.unsubscribe(subscriptionId) end

---@class Camera
Camera = {}

---@param pos Vector3
function Camera.set_position(pos) end

---@return Vector3
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
