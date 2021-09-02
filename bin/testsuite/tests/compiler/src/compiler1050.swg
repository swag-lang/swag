func(T) new()->*T
{
	var req : Swag.AllocatorRequest
	req.size = @sizeof(T)
	@getcontext().allocator.alloc(&req)

	@init(cast(*T) req.address)("yes", "no")

	return acast req.address
}

struct Point
{
	x, y: string = "coucou"
}
//#[Swag.PrintBc]
#test
{
	ptr := new'Point()
	@assert(ptr.x == "yes")
	@assert(ptr.y == "no")
}