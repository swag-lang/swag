

struct(T) Array
{
	buffer: *T
	count:	u32
	size:	u32
}

impl Array
{
	func add(using self, value: T)
	{
		buffer[0] = value
	}
}

struct Point
{
	x: s32
}

//#[swag.printbc]
#test
{
	var pt: Point
	var arr: Array'Point
	arr.buffer = &pt
	arr.add(@{1})
	@assert(arr.buffer[0].x == 1)
}