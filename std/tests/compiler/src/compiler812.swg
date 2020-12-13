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
		if buffer != null
			@memcpy(newPtr, buffer, size)
		size = newSize
		@free(buffer)
		buffer = cast(*u8) newPtr
	}

	func opAffect(using self, from: string)
	{
		length = @countof(from)
		self.reserveSize(length + 1)
		@memcpy(buffer, @dataof(from), length + 1)
	}
}

//#[swag.printbc]
#test
{
	var str: Utf8;
	str	= "toto"
}