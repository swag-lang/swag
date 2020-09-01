namespace std.input
{
    using std.math

    // Represents one mouse state
    public struct MouseState
    {
        // Whether each button is pressed or not
        pressed:    [@countof(MouseButton)] bool
        // Mouse position, in screen coordinates
        position:   Point
    }

    // Represents a mouse state with cursor position and button press information
    impl MouseState
    {
        public
        {
            // Compute the actual state of the mouse
            func update(using self)
            {
                pressed[cast(u32) MouseButton.Left] = isMouseButtonPressed(MouseButton.Left)
                pressed[cast(u32) MouseButton.Middle] = isMouseButtonPressed(MouseButton.Middle)
                pressed[cast(u32) MouseButton.Right] = isMouseButtonPressed(MouseButton.Right)
                pressed[cast(u32) MouseButton.XButton1] = isMouseButtonPressed(MouseButton.XButton1)
                pressed[cast(u32) MouseButton.XButton2] = isMouseButtonPressed(MouseButton.XButton2)
                position = getMousePos()
            }

            // Reset the state to its default value
            func clear(using self)
            {
                memory.clear(&pressed, @sizeof(pressed))
                position = getMousePos()
            }
        }
    }

    // Determins if one given mouse button is pressed or not
	public func isMouseButtonPressed(button: MouseButton)->bool
	{
		using input, MouseButton
		switch button
		{
		case Left:		return isKeyPressed(Key.LButton)
		case Middle:	return isKeyPressed(Key.MButton)
		case Right:		return isKeyPressed(Key.RButton)
		case XButton1:	return isKeyPressed(Key.XButton1)
		case XButton2:	return isKeyPressed(Key.XButton2)
		}

		return false
	}
}
