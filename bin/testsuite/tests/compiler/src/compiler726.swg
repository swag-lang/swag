struct iter
{
    value:     s32
    end:       s32
    arr:       *Array
}

impl iter
{
    func next(me)->s32
    {
        if (me.value != me.end)
        {
            me.value += 1
            return 0 //me.arr[me.value - 1]
        }

        return 0
    }
}

struct Array {}

impl Array
{
    func getIter(me)->iter
    {
        var ret: iter
        ret.end = 10
        return ret
    }
}

//#[Swag.PrintBc]
/*#test
{
	var arr: Array
	var it = arr.getIter()
}*/
