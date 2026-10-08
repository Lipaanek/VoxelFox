local self = setmetatable({}, Node)

function ready()
    print("Attached to node:", self.get_name())

    local position = self.get_position()
    self.set_position(Vector3.new(position.x, position.y, position.z))
end

function update(dt)
    -- Example of positioning object using code
    -- This example won't work in the current state of engine,
    -- Due to the commented out update call in Editor
    -- (because editor doesn't run non-editor scripts, use !editor for editor scripts
    --local position = self.get_position()
    --self.set_position(Vector3.new(position.x, position.y + 2 * dt, position.z))
end