/*namespace std.utf8
{
	public func getChar(buffer: *u8)->{*u8, char}
	{
		c := deref buffer
		if c == 0
			return {buffer, acast c}
			
		buffer += 1		
		if ((c & 0x80) == 0)
		{
			return {buffer, acast c}
		}
			
		var wc: char = ?
        if ((c & 0xE0) == 0xC0)
        {
            wc = (c & 0x1F) << 6;
			c = deref buffer
			buffer += 1		
            wc |= (c & 0x3F);
            return {buffer, wc}
        }
		
		return {buffer, 0}
	}
}*/