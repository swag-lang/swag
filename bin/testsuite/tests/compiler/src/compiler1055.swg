using Swag

struct(T) Array
{
	buffer: ^T
	count:	u32
	size:	u32
}

impl Array
{
	mtd opDrop()
	{
		var request: AllocatorRequest
		request.mode = AllocatorMode.Free
		request.address = buffer
		request.size = cast(u64) size * @sizeof(T)
		@getcontext().allocator.req(&request)
	}

	func validateCount(self, newCount: u32)
	{
		if(newCount < self.size)
			return;

		self.size = self.size * 2
		if self.size < 8
			self.size = 8

		let context = @getcontext()

		var request: AllocatorRequest
		request.size = cast(u64) self.size * @sizeof(T)
		context.allocator.req(&request)
		let newBuffer = request.address
		if self.buffer
			@memcpy(newBuffer, self.buffer, cast(u64) self.count * @sizeof(T))

		request.mode = AllocatorMode.Free
		request.address = self.buffer
		context.allocator.req(&request)

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