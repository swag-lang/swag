struct(T) Array
{
	buffer: ^T
	count:	u32
	size:	u32
}

impl Array
{
    //#[Swag.PrintBc, Swag.Optimize("", false)]
	func add(using self, value: const &T)
	{
		buffer[0] = value
	}
}

struct Point
{
	x: s32
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	var arr: Array'Point
	arr.buffer = &pt
	arr.add({1})
	@assert(arr.buffer[0].x == 1)
}