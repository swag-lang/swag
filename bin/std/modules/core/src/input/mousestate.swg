#global public
#global namespace Input
using Math

// Represents one mouse state
struct MouseState
{
    pressed:         [@countof(MouseButton)] bool
    pressedTime:     [@countof(MouseButton)] u32
    pressedPos:      [@countof(MouseButton)] Point
    position:        Point
}

// Represents a mouse state with cursor position and button press information
impl MouseState
{
    // Compute the actual state of the mouse
    mtd update()
    {
        .position = getMousePos()

        foreach v in MouseButton
        {
            let b = .pressed[v]
            .pressed[v] = Mouse.isButtonPressed(v)
            if !b and .pressed[v]
            {
                .pressedTime[v] = Time.nowMilliseconds()
                .pressedPos[v]  = .position
            }
        }
    }

    // Reset the state to its default value
    mtd clear()
    {
        Memory.clear(&.pressed, #sizeof(.pressed))
        .position = getMousePos()
    }
}
