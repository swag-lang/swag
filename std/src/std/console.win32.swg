namespace std.console
{
	using os.win32
	
	var g_colorToWin: [@countof(Color)] u16
	
	#[swag.compiler]
	func colorToWin()
	{
		using Color
		g_colorToWin[cast(u32) Black] 		= 0
		g_colorToWin[cast(u32) White] 		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
		g_colorToWin[cast(u32) Gray]  		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
		g_colorToWin[cast(u32) Red]  		= FOREGROUND_RED | FOREGROUND_INTENSITY
		g_colorToWin[cast(u32) Green] 		= FOREGROUND_GREEN | FOREGROUND_INTENSITY
		g_colorToWin[cast(u32) Blue]  		= FOREGROUND_BLUE | FOREGROUND_INTENSITY
		g_colorToWin[cast(u32) Cyan]  		= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
		g_colorToWin[cast(u32) Magenta] 	= FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY
		g_colorToWin[cast(u32) Yellow] 		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
		g_colorToWin[cast(u32) DarkRed] 	= FOREGROUND_RED
		g_colorToWin[cast(u32) DarkGreen] 	= FOREGROUND_GREEN
		g_colorToWin[cast(u32) DarkBlue] 	= FOREGROUND_BLUE
		g_colorToWin[cast(u32) DarkCyan] 	= FOREGROUND_BLUE | FOREGROUND_GREEN
		g_colorToWin[cast(u32) DarkMagenta] = FOREGROUND_BLUE | FOREGROUND_RED
		g_colorToWin[cast(u32) DarkYellow] 	= FOREGROUND_RED | FOREGROUND_GREEN
	}
	
	var g_stdOutputInitAttr: u16
	var g_stdOutputCurAttr: u16
	
	#run 
	{ 
		colorToWin()
	}
	
	func init()
	{
		var csbi: CONSOLE_SCREEN_BUFFER_INFO
		GetConsoleScreenBufferInfo(g_stdOutputHandle, csbi)		
		g_stdOutputInitAttr = csbi.wAttributes
		g_stdOutputCurAttr = g_stdOutputInitAttr
	}
	
	func drop()
	{
		resetColor()
	}

	#[swag.public]
	func setTextColor(color: Color)
	{
		attributes := g_colorToWin[cast(u32) color]
		g_stdOutputCurAttr &= BACKGROUND_MASK
		g_stdOutputCurAttr |= attributes
		SetConsoleTextAttribute(g_stdOutputHandle, g_stdOutputCurAttr)
	}
	
	#[swag.public]
	func setBackColor(color: Color)
	{
		attributes := cast(u16) (g_colorToWin[cast(u32) color] << 4)
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