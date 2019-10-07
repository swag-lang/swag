#module module1051

func(T, N: u32) new()->*T
{
	var req : swag.AllocatorRequest
	req.size = @sizeof(T) * N
	@getcontext().allocator(req)
	
	init(cast(*T) req.address, N)
	
	return acast req.address
}

func(T, N: u32) delete(addr: *T)
{
	drop(addr, N)
	
	var req : swag.AllocatorRequest
	req.size = @sizeof(T) * N
	req.address = addr
	req.mode = swag.AllocatorMode.Free
	@getcontext().allocator(req)
}

struct Point
{
	x, y: string = "coucou"
}

//#[swag.printbc]
#test
{
	ptr := new!(Point, 4)()
	delete!(Point, 4)(ptr)
}