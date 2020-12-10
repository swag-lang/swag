

struct Utf8
{
	buffer: *u8
	length:	u32
	size:	u32
}

impl Utf8
{
	func reserveSize(using self, newSize: u32)
	{
		if size >= newSize
			return;

		newPtr := @alloc(cast(uint) newSize)
		if buffer
			@memcpy(newPtr, buffer, cast(uint) size)
		size = newSize
		@free(buffer)
		buffer = cast(*u8) newPtr
	}

	func opAffect(using self, from: string)
	{
		length = @countof(from)
		self.reserveSize(length + 1)
		@memcpy(buffer, @dataof(from), cast(uint) length + 1)
	}
}

//#[swag.printbc]
#test
{
	var str: Utf8 = "toto"
	@assert(str.length == 4)
	@assert(str.size == 5)
}