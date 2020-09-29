namespace std.mouse;
using std.math

// Represents one mouse state
public struct State
{
    // Whether each button is pressed or not
    pressed:    [@countof(Button)] bool
    // Mouse position, in screen coordinates
    position:   Point
}

// Represents a mouse state with cursor position and button press information
public impl State
{
    // Compute the actual state of the mouse
    func update(using self)
    {
        pressed[cast(u32) Button.Left] = isButtonPressed(Button.Left)
        pressed[cast(u32) Button.Middle] = isButtonPressed(Button.Middle)
        pressed[cast(u32) Button.Right] = isButtonPressed(Button.Right)
        pressed[cast(u32) Button.XButton1] = isButtonPressed(Button.XButton1)
        pressed[cast(u32) Button.XButton2] = isButtonPressed(Button.XButton2)
        position = getPos()
    }

    // Reset the state to its default value
    func clear(using self)
    {
        memory.clear(&pressed, @sizeof(pressed))
        position = getPos()
    }
}

// Determins if one given mouse button is pressed or not
public func isButtonPressed(button: Button)->bool
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
