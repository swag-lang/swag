namespace std.input
{
    using std.math
    public struct MouseState
    {
        pressed:    [@countof(MouseButton)] bool
        position:   Point
    }

    impl MouseState
    {
        public
        {
            func update(using self)
            {
                pressed[cast(u32) MouseButton.Left] = isMouseButtonPressed(MouseButton.Left)
                pressed[cast(u32) MouseButton.Middle] = isMouseButtonPressed(MouseButton.Middle)
                pressed[cast(u32) MouseButton.Right] = isMouseButtonPressed(MouseButton.Right)
                pressed[cast(u32) MouseButton.XButton1] = isMouseButtonPressed(MouseButton.XButton1)
                pressed[cast(u32) MouseButton.XButton2] = isMouseButtonPressed(MouseButton.XButton2)
                position = getMousePos()
            }
            
            func clear(using self)
            {
                memory.clear(&pressed, @sizeof(pressed))
                position = getMousePos()
            }
            
            func isButtonPressed(using self, button: MouseButton)->bool
            {
                return pressed[cast(u32) button]
            }
            
            func isButtonReleased(using self, button: MouseButton)->bool
            {
                return !pressed[cast(u32) button]
            }            
        }
    }

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
