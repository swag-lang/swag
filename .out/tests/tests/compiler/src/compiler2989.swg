struct(T) AA
{
}

impl AA
{
    func index(using self, i: uint)->bool
    {
        return false
    }
    func opIndex(using self, i: uint)->bool
    {
        return false
    }
}

func A()
{
    var x: AA'{s32}
    v := x.index(0)
    //v1 := x[0]
}

func B()
{
    var x: AA'{s32}
    v := x.index(0)
    //v1 := x[0]
}