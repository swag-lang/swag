#if swag.WINDOWS
{
	namespace std.mouse
	{
		public func isButtonPressed(button: Button)->bool
		{
			using keyboard
			using Button
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
}