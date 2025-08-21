struct(T) Array
{
}

impl Array
{
	func add(using me, value: T)
	{
	}
}

//#[Swag.PrintBc]
#test
{
	var arr: Array's32
	arr.add(666)
}