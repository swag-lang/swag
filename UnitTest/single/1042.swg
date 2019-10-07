#module module1042

func(T) new()->*T
{
	var req : swag.AllocatorRequest
	req.size = @sizeof(T)
	
	context := @getcontext() 
	context.allocator(req)
	
	init(cast(*T) req.address)
	return acast req.address
}

struct Point
{
	x, y: string = "coucou"
}
//#[swag.printbc]
#test
{
	ptr := new!Point()
	@assert(ptr.x == "coucou")
	@assert(ptr.y == "coucou")
}