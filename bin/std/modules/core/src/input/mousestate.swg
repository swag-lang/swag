#global public
#global namespace Input
using Math

// Represents one mouse state
struct MouseState
{
    pressed:        [@countof(MouseButton)] bool
    pressedTime:    [@countof(MouseButton)] u32
    position:       Point
}

// Represents a mouse state with cursor position and button press information
impl MouseState
{
    // Compute the actual state of the mouse
    mtd update()
    {
        visit v: MouseButton
        {
            b := pressed[v]
            pressed[v] = Mouse.isButtonPressed(v)
            if !b and pressed[v]
                pressedTime[v] = Time.nowMilliseconds()
        }

        position = getMousePos()
    }

    // Reset the state to its default value
    mtd clear()
    {
        Memory.clear(&pressed, @sizeof(pressed))
        position = getMousePos()
    }
}