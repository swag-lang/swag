using Swag

//#[Swag.PrintBc]
#test
{
	var context = @getcontext()

	var req: AllocatorRequest
	req.size = 16

	context.allocator.req(&req)
	req.mode = AllocatorMode.Free
	context.allocator.req(&req)
}