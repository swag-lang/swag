

#global testerror
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

//#[Swag.printbc]
#test
{
	var arr: Array!Point
	arr.add(1.0)
}