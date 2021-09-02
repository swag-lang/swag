struct(T) Array
{
	buffer: *T
	count:	u32
	size:	u32
}

impl Array
{
	func validateCount(self, newCount: u32)
	{
		using Swag

		if(newCount < self.size)
			return;

		self.size = self.size * 2
		if self.size < 8
			self.size = 8

		context := @getcontext()

		var request: AllocatorRequest
		request.size = cast(uint) self.size * @sizeof(T)
		context.allocator.alloc(&request)
		newBuffer := request.address
		if self.buffer
			@memcpy(newBuffer, self.buffer, cast(uint) self.count * @sizeof(T))

		request.mode = AllocatorMode.Free
		request.address = self.buffer
		context.allocator.alloc(&request)

		self.buffer = acast newBuffer
	}

	func add(using self, value: T)
	{
		validateCount(self, self.count + 1)
		self.buffer[self.count] = value
		self.count += 1
	}
}

//#[Swag.PrintBc]
#test
{
	var arr: Array's32
	arr.add(666)
	@assert(arr.buffer[0] == 666)
}