

struct(T) Array
{
	buffer: *T
	count:	u32
	size:	u32
}

impl Array
{
	func add(using self)
	{
		buffer[0] = 666
		//count = 0
	}
}

//#[swag.printbc]
#test
{
	x := 0
	var arr: Array's32
	arr.buffer = &x
	arr.add()
	@assert(x == 666)
}