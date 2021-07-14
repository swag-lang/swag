

func(T, N: u32) new()->*T
{
	var req : Swag.AllocatorRequest
	req.size = @sizeof(T) * N
	@getcontext().allocator.alloc(&req)

	@init(cast(*T) req.address, N)

	return acast req.address
}

func(T, N: u32) delete(addr: *T)
{
	@drop(addr, N)

	var req : Swag.AllocatorRequest
	req.size = @sizeof(T) * N
	req.address = addr
	req.mode = Swag.AllocatorMode.Free
	@getcontext().allocator.alloc(&req)
}

struct Point
{
	x, y: string = "coucou"
}

var G: s32 = 0

impl Point
{
	func opDrop(self)
	{
		G += 1
	}
}

//#[Swag.PrintBc]
#test
{
	G = 0
	ptr := new'(Point, 4)()
	delete'(Point, 4)(ptr)
	@assert(G == 4)
}