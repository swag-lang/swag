func(T) new()->*T
{
	var req : swag.AllocatorRequest
	req.size = @sizeof(T)
	@getcontext().allocator.alloc(&req)

	@init(cast(*T) req.address)("yes", "no")

	return autocast req.address
}

struct Point
{
	x, y: string = "coucou"
}
//#[swag.printbc]
#test
{
	ptr := new'Point()
	@assert(ptr.x == "yes")
	@assert(ptr.y == "no")
}