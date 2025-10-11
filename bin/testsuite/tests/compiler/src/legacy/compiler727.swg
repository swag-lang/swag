/*
struct iter
{
	var value: 	s32
	var end: 	s32
	var arr: 	*Array
}

impl iter
{
	func next(me)->s32
	{
		if(me.value != me.end)
		{
			me.value += 1
			return me.arr.values[me.value - 1]
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
	func getIter(me)->iter
	{
		var ret: iter
		ret.end = 3
		ret.arr = &me
		return ret
	}
}

//#[Swag.PrintBc]
#test
{
	var arr: Array
	arr.values[0] = 10
	var it = arr.getIter()
	@print(it.arr.values[0])
}
*/
