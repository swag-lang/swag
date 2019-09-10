#module module798

struct Utf8
{
	buffer: *u8
	length:	u32
	size:	u32
}

impl Utf8
{
	func reserveSize(self, size: u32)
	{
		if(self.size >= size)
			return;
			
		newPtr := @alloc(size)
		if self.buffer != null
			@memcpy(newPtr, self.buffer, self.size)
		self.size = size;
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

//#[swag.printbc]
#test
{
	var str: Utf8;
	str	= "toto"
}