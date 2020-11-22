

struct(T) Array
{
	buffer: *T
	count:	u32
	size:	u32
}

impl Array
{
	func validateCount(using self, newCount: u32)
	{
		using swag

		if(newCount < size)
			return;

		size = size * 2
		if size < 8
			size = 8

		context := @getcontext()

		var request: AllocatorRequest
		request.size = size * @sizeof(T)
		context.allocator.alloc(&request)
		newBuffer := request.address

		if buffer
			@memcpy(newBuffer, buffer, count * @sizeof(T))

		request.mode = AllocatorMode.Free
		request.address = buffer
		context.allocator.alloc(&request)

		buffer = autocast newBuffer
	}

	func add(using self, value: T)
	{
		validateCount(self, count + 1)
		buffer[count] = value
		count += 1
	}
}

//#[swag.printbc]
#test
{
	var arr: Array's32
	arr.add(666)
	@assert(arr.buffer[0] == 666)
}