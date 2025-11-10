struct AA
{
    x:   s64 = 1
}

impl AA
{
    func opDrop(me)
    {
        //@print(x)
        //@assert(x == 1)
        me.x = 666
        //@print("drop")
    }
}

func toto()->{ x: AA }
{
    var result: retval
    return result
}

//#[Swag.PrintBc]
#test
{
    var (v) = toto()
    //var v1 = v.item0
}
