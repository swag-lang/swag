struct Utf8
{
	buffer: *u8
	length:	u64
	size:	u64
}

impl Utf8
{
	func reserveSize(using self, newSize: u64)
	{
		if size >= newSize do
			return;

		let newPtr = @alloc(newSize)
		if buffer do
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

//#[Swag.PrintBc]
#test
{
	var str: Utf8 = "toto"
	@assert(str.length == 4)
	@assert(str.size == 5)
}