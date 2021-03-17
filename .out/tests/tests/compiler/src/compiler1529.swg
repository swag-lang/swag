struct(T) Array
{
}

impl Array
{
	func add(using self, value: T)
	{
	}
}

//#[swag.printbc]
#test
{
	var arr: Array's32
	arr.add(666)
}