using Swag

//#[Swag.PrintBc]
#test
{
	context := @getcontext()

	var req: AllocatorRequest
	req.size = 16

	context.allocator.alloc(&req)
	req.mode = AllocatorMode.Free
	context.allocator.alloc(&req)
}