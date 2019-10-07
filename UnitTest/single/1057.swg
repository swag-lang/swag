#module module1057

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
		buffer[0] = 0
		//count = 0
	}
}

//#[swag.printbc]
#test
{
	var arr: Array!s32
	arr.add()
}