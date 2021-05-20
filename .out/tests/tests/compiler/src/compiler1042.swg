func(T) new()->*T
{
	var req : Swag.AllocatorRequest
	req.size = @sizeof(T)

	context := @getcontext()
	context.allocator.alloc(&req)

	@init(cast(*T) req.address)
	return acast req.address
}

struct Point
{
	x, y: string = "coucou"
}
//#[Swag.printbc]
#test
{
	ptr := new'Point()
	@assert(ptr.x == "coucou")
	@assert(ptr.y == "coucou")
}