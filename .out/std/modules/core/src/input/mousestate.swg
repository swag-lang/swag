#global public
#global namespace Input
using Math

// Represents one mouse state
struct MouseState
{
    pressed:    [@countof(MouseButton)] bool
    position:   Point
}

// Represents a mouse state with cursor position and button press information
impl MouseState
{
    // Compute the actual state of the mouse
    method update()
    {
        using MouseButton
        pressed[Left] = isMouseButtonPressed(Left)
        pressed[Middle] = isMouseButtonPressed(Middle)
        pressed[Right] = isMouseButtonPressed(Right)
        pressed[XButton1] = isMouseButtonPressed(XButton1)
        pressed[XButton2] = isMouseButtonPressed(XButton2)
        position = getMousePos()
    }

    // Reset the state to its default value
    method clear()
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
