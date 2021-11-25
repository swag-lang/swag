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
            pressed[v] = isMouseButtonPressed(v)
            if !b and pressed[v]
                pressedTime[v] = Time.nowMilliseconds()
        }

        position = getMousePos()
    }

    // Reset the state to its default value
    mtd clear()
    {
        Memory.clear(&pressed, cast(uint) @sizeof(pressed))
        position = getMousePos()
    }
}

// Determins if one given mouse button is pressed or not
func isMouseButtonPressed(button: MouseButton)->bool
{
    using MouseButton
    switch button
    {
    case Left:		return Input.isKeyPressed(Input.Key.LButton)
    case Middle:	return Input.isKeyPressed(Input.Key.MButton)
    case Right:		return Input.isKeyPressed(Input.Key.RButton)
    case XButton1:	return Input.isKeyPressed(Input.Key.XButton1)
    case XButton2:	return Input.isKeyPressed(Input.Key.XButton2)
    }

    return false
}
