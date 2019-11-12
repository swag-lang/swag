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
    
    private var silentMode: u32
    
    public
    {
        func silent(value: bool)
        {
            silentMode += value ? 1 : -1
        }
    }
}