struct(T) Array
{
}

impl Array
{
	func add(using self, value: T)
	{
	}
}

//#[Swag.printbc]
#test
{
	var arr: Array's32
	arr.add(666)
}