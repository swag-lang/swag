struct Utf8
{
	buffer: *u8
	length:	uint
	size:	uint
}

impl Utf8
{
	func reserveSize(self, newSize: uint)
	{
		if(self.size >= newSize)
			return;

		newPtr := @alloc(newSize)
		if self.buffer != null
			@memcpy(newPtr, self.buffer, self.size)
		self.size = newSize;
		@free(self.buffer)
		self.buffer = cast(*u8) newPtr;
	}

	func opAffect(self, from: string)
	{
		self.length = @countof(from)
		self.reserveSize(self.length + 1)
		@memcpy(self.buffer, @dataof(from), self.length + 1)
	}
}

//#[Swag.printbc]
#test
{
	var str: Utf8;
	str	= "toto"
}