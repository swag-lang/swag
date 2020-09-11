struct(T) Array
{
}

impl Array
{
	func validateCount()
	{
	    size := @sizeof(T)
    }

	func add(self)
	{
		validateCount()
	}
}

//#[swag.printbc]
#test
{
	var arr: Array's32
	arr.add()
}