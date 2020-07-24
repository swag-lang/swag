#[swag.printbc]
#test
{
    var req : swag.AllocatorRequest
	req.size = 10

	context := @getcontext()
	context.allocator.alloc(&req)

	/*init(cast(*T) req.address)
	return acast req.address*/
}