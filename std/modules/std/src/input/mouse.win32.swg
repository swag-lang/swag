#if swag.WINDOWS
{
	namespace std.input
	{
		using os

		public 
		{
			func getMousePos()->(s32, s32)
			{
				var pt: POINT = ?
				GetCursorPos(&pt)
				return {pt.x, pt.y}
			}

			func setMousePos(x, y: s32)
			{
				SetCursorPos(x, y)
			}

			func showMouseCursor(show: bool)
			{
				ShowCursor(cast(BOOL) show)
			}
		}
	}
}