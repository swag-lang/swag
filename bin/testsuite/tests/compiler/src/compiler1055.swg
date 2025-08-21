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
		request.size = cast(u64) size * #sizeof(T)
		@getcontext().allocator.req(&request)
	}

	func validateCount(me, newCount: u32)
	{
		if newCount < me.size:
			return;

		me.size = me.size * 2
		if me.size < 8:
			me.size = 8

		let context = @getcontext()

		var request: AllocatorRequest
		request.size = cast(u64) me.size * #sizeof(T)
		context.allocator.req(&request)
		let newBuffer = request.address
		if me.buffer:
			@memcpy(newBuffer, me.buffer, cast(u64) me.count * #sizeof(T))

		request.mode = AllocatorMode.Free
		request.address = me.buffer
		context.allocator.req(&request)

		me.buffer = cast() newBuffer
	}

	func add(using me, value: T)
	{
		validateCount(me, me.count + 1)
		me.buffer[me.count] = value
		me.count += 1
	}
}

//#[Swag.PrintBc]
#test
{
	var arr: Array's32
	arr.add(666)
	@assert(arr.buffer[0] == 666)
}