namespace std.console
{
	public enum Color
	{
		Black
		White
		Gray
		Red
		Blue
		Green
		Cyan
		Yellow
		Magenta
		DarkRed
		DarkBlue
		DarkGreen
		DarkCyan
		DarkYellow
		DarkMagenta
	}
    
    var silentMode: u32
    
    public
    {
        func silent(value: bool)
        {
            diagnostics.debug.assert(value || silentMode > 0)
            silentMode += value ? 1 : -1
        }
    }
}