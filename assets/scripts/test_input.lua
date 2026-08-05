-- Test file

function on_ready()
    print("Ready function called!")
end

function update(dt)
    if Input.is_active("move_forward") then
        print("move_forward is active")
    end
end
