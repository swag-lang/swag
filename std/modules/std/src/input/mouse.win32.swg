#if swag.WINDOWS
{
	namespace std.input
	{
		using std.os, std.math

		public
		{
			func getMousePos()->Point
			{
				var pt: POINT = ?
				GetCursorPos(&pt)
				return Point(cast(f32) pt.x, cast(f32) pt.y)
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
