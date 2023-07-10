struct Utf8
{
	buffer: *u8
	length:	u64
	size:	u64
}

impl Utf8
{
	func reserveSize(self, newSize: u64)
	{
		if(self.size >= newSize)
			return;

		let newPtr = @alloc(newSize)
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

//#[Swag.PrintBc]
#test
{
	var str: Utf8;
	str	= "toto"
}