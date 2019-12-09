namespace std.input
{
    public enum MouseButton
    {
		Left
		Middle
		Right
		XButton1
		XButton2
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
