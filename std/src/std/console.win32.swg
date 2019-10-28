namespace std.console
{
	using os.win32
	
	func colorToWin(color: Color)->u16
	{
		using Color
		
		var attributes: u16
		switch color
		{
		case Black:
			attributes = 0
		case White:
			attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
		case Gray:
			attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
		case Red:
			attributes = FOREGROUND_RED | FOREGROUND_INTENSITY
		case Green:
			attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY
		case Blue:
			attributes = FOREGROUND_BLUE | FOREGROUND_INTENSITY
		case Cyan:
			attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
		case Magenta:
			attributes = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY
		case Yellow:
			attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
		case DarkRed:
			attributes = FOREGROUND_RED
		case DarkGreen:
			attributes = FOREGROUND_GREEN
		case DarkBlue:
			attributes = FOREGROUND_BLUE
		case DarkCyan:
			attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
		case DarkMagenta:
			attributes = FOREGROUND_BLUE | FOREGROUND_RED
		case DarkYellow:
			attributes = FOREGROUND_RED | FOREGROUND_GREEN
		}	
		
		return attributes
	}
	
	var g_stdOutputInitAttr: u16
	var g_stdOutputCurAttr: u16
	
	func setup()
	{
		var csbi: CONSOLE_SCREEN_BUFFER_INFO
		GetConsoleScreenBufferInfo(g_stdOutputHandle, csbi)		
		g_stdOutputInitAttr = csbi.wAttributes
		g_stdOutputCurAttr = g_stdOutputInitAttr
	}

	#[swag.public]
	func setTextColor(color: Color)
	{
		attributes := colorToWin(color)
		g_stdOutputCurAttr &= BACKGROUND_MASK
		g_stdOutputCurAttr |= attributes
		SetConsoleTextAttribute(g_stdOutputHandle, g_stdOutputCurAttr)
	}
	
	#[swag.public]
	func setBackColor(color: Color)
	{
		attributes := cast(u16) (colorToWin(color) << 4)
		g_stdOutputCurAttr &= FOREGROUND_MASK
		g_stdOutputCurAttr |= attributes		
		SetConsoleTextAttribute(g_stdOutputHandle, g_stdOutputCurAttr)
	}

	#[swag.public]
	func resetColor()
	{
		SetConsoleTextAttribute(g_stdOutputHandle, g_stdOutputInitAttr)
	}
}