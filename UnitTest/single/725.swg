#module module725

struct iter
{
	var value: s32
	var end: s32
	var arr: *Array
}

impl iter
{
	func next(self)->s32
	{
		if(self.value != self.end)
		{
			self.value += 1
			return self.arr[self.value - 1]
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
	
//#[swag.printbc]
#test
{
	var arr: Array
	it := arr.getIter()
}