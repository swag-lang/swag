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
        me.position = getMousePos()

        foreach v in MouseButton
        {
            let b = me.pressed[v]
            me.pressed[v] = Mouse.isButtonPressed(v)
            if !b and me.pressed[v]
            {
                me.pressedTime[v] = Time.nowMilliseconds()
                me.pressedPos[v]  = me.position
            }
        }
    }

    // Reset the state to its default value
    mtd clear()
    {
        Memory.clear(&me.pressed, #sizeof(me.pressed))
        me.position = getMousePos()
    }
}
