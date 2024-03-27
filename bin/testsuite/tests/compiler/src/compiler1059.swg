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

	func validateCount(using self, newCount: u32)
	{
		if newCount < size do
			return

		size = size * 2
		if size < 8 do
			size = 8

		let context = @getcontext()

		var request: AllocatorRequest
		request.size = cast(u64) size * @sizeof(T)
		context.allocator.req(&request)
		let newBuffer = request.address

		if buffer do
			@memcpy(newBuffer, buffer, cast(u64) count * @sizeof(T))

		request.mode = AllocatorMode.Free
		request.address = buffer
		context.allocator.req(&request)

		buffer = acast newBuffer
	}

	func add(using self, value: T)
	{
		validateCount(self, count + 1)
		buffer[count] = value
		count += 1
	}
}

//#[Swag.PrintBc]
#test
{
	var arr: Array's32
	arr.add(666)
	@assert(arr.buffer[0] == 666)
}