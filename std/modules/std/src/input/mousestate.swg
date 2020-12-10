public namespace mouse;
using math

// Represents one mouse state
struct State
{
    // Whether each button is pressed or not
    pressed:    [@countof(Button)] bool
    // Mouse position, in screen coordinates
    position:   Point
}

// Represents a mouse state with cursor position and button press information
impl State
{
    // Compute the actual state of the mouse
    func update(using self)
    {
        using Button
        pressed[Left] = isButtonPressed(Left)
        pressed[Middle] = isButtonPressed(Middle)
        pressed[Right] = isButtonPressed(Right)
        pressed[XButton1] = isButtonPressed(XButton1)
        pressed[XButton2] = isButtonPressed(XButton2)
        position = getPos()
    }

    // Reset the state to its default value
    func clear(using self)
    {
        memory.clear(&pressed, cast(uint) @sizeof(pressed))
        position = getPos()
    }
}

// Determins if one given mouse button is pressed or not
func isButtonPressed(button: Button)->bool
{
    using Button
    switch button
    {
    case Left:		return keyboard.isKeyPressed(keyboard.Key.LButton)
    case Middle:	return keyboard.isKeyPressed(keyboard.Key.MButton)
    case Right:		return keyboard.isKeyPressed(keyboard.Key.RButton)
    case XButton1:	return keyboard.isKeyPressed(keyboard.Key.XButton1)
    case XButton2:	return keyboard.isKeyPressed(keyboard.Key.XButton2)
    }

    return false
}
