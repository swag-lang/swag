

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
		if(size >= newSize)
			return;

		newPtr := @alloc(cast(uint) newSize)
		if buffer
			@memcpy(newPtr, buffer, cast(uint) size)
		size = newSize
		@free(buffer)
		buffer = autocast newPtr
	}

	func opAffect(using self, from: string)
	{
		length = @countof(from)
		self.reserveSize(length + 1)
		if length
			@memcpy(buffer, @dataof(from), cast(uint) length + 1)
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