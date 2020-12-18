

struct(T) Array
{
	count:	u32
}

impl Array
{
	func add(using self)
	{
		count += 1
	}
}

//#[swag.printbc]
#test
{
	var arr: Array's32
	arr.add()
}