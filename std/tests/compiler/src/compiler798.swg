

struct Utf8
{
	buffer: *u8
	length:	u32
	size:	u32
}

impl Utf8
{
	func reserveSize(self, newSize: u32)
	{
		if(self.size >= newSize)
			return;

		newPtr := @alloc(cast(uint) newSize)
		if self.buffer != null
			@memcpy(newPtr, self.buffer, cast(uint) self.size)
		self.size = newSize;
		@free(self.buffer)
		self.buffer = cast(*u8) newPtr;
	}

	func opAffect(self, from: string)
	{
		self.length = @countof(from)
		self.reserveSize(self.length + 1)
		@memcpy(self.buffer, @dataof(from), cast(uint) self.length + 1)
	}
}

//#[swag.printbc]
#test
{
	var str: Utf8;
	str	= "toto"
}