public func getChar(buffer: *u8)->{*u8, char}
{
	ptr := buffer
	c := deref ptr
	if c == 0
		return {ptr, acast 0}
		
	ptr += 1		
	if ((c & 0x80) == 0)
	{
		return {ptr, acast c}
	}
		
	var wc: char = ?
	if ((c & 0xE0) == 0xC0)
	{
		wc = (c & 0x1F) << 6;
		c = deref ptr
		ptr += 1		
		wc |= (c & 0x3F);
		return {ptr, wc}
	}
	
	return {ptr, acast 0}
}
