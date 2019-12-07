#if swag.WINDOWS
{
	namespace std.mouse
	{
		using os

		public 
		{
			func getPos()->(s32, s32)
			{
				var pt: POINT = ?
				GetCursorPos(&pt)
				return {pt.x, pt.y}
			}

			func setPos(x, y: s32)
			{
				SetCursorPos(x, y)
			}

			func showCursor(show: bool)
			{
				ShowCursor(cast(BOOL) show)
			}
		}
	}
}