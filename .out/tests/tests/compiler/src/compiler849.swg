struct Utf8
{
	buffer: *u8
	length:	uint
	size:	uint
}

impl Utf8
{
	func reserveSize(using self, newSize: uint)
	{
		if(size >= newSize)
			return;

		newPtr := @alloc(newSize)
		if buffer
			@memcpy(newPtr, buffer, size)
		size = newSize
		@free(buffer)
		buffer = acast newPtr
	}

	func opAffect(using self, from: string)
	{
		length = @countof(from)
		self.reserveSize(length + 1)
		if length
			@memcpy(buffer, @dataof(from), length + 1)
		else
			buffer[0] = 0
	}
}

//#[swag.printbc]
#test
{
	var str: Utf8 = null
	var str1: Utf8 = "null"
}