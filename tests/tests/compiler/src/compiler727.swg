
/*
struct iter
{
	var value: 	s32
	var end: 	s32
	var arr: 	*Array
}

impl iter
{
	func next(self)->s32
	{
		if(self.value != self.end)
		{
			self.value += 1
			return self.arr.values[self.value - 1]
		}
		
		return 0
	}
}

struct Array
{
	var values: [3] s32
}

impl Array
{
	func getIter(self)->iter
	{
		var ret: iter
		ret.end = 3
		ret.arr = &self
		return ret
	}
}
	
//#[swag.printbc]
#test
{
	var arr: Array
	arr.values[0] = 10
	it := arr.getIter()
	@print(it.arr.values[0])
}
*/