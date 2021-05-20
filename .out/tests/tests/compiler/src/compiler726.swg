

struct iter
{
	value: s32
	end: s32
	arr: *Array
}

impl iter
{
	func next(self)->s32
	{
		if(self.value != self.end)
		{
			self.value += 1
			return 0//self.arr[self.value - 1]
		}

		return 0
	}
}

struct Array
{
}

impl Array
{
	func getIter(self)->iter
	{
		var ret: iter
		ret.end = 10
		return ret
	}
}

//#[Swag.printbc]
/*#test
{
	var arr: Array
	it := arr.getIter()
}*/