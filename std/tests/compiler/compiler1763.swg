using swag

//#[swag.printbc]
#test
{
	context := @getcontext()
    newcxt := deref context
    @setcontext(newcxt)
    defer @setcontext(context)

	var req: AllocatorRequest
	req.size = 16

	context.allocator.alloc(&req)
	req.mode = AllocatorMode.Free
	context.allocator.alloc(&req)
}