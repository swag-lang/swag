#[swag.printbc]
#test
{
    var req : swag.AllocatorRequest
	req.size = 666

	context := @getcontext()
	context.allocator.alloc(&req)

	/*init(cast(*T) req.address)
	return acast req.address*/
}
