struct(T) AA
{
}

impl AA
{
    func index(using self, i: u64)->bool
    {
        return false
    }
    func opIndex(using self, i: u64)->bool
    {
        return false
    }
}

func A()
{
    var x: AA'{a:   s32}
    var v  = x.index(0)
    var v1 = x[0]
}

func B()
{
    var x: AA'{a:   s32}
    var v  = x.index(0)
    var v1 = x[0]
}
