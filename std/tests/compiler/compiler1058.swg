

struct(T) Array
{
	buffer: *T
	count:	u32
	size:	u32
}

impl Array
{
	func validateCount(self, newCount: u32)
	{
	}
	
	func add(using self, value: T)
	{
		validateCount(self, self.count + 1)
	}
}

//#[swag.printbc]
#test
{
	var arr: Array's32
	arr.add(666)
}