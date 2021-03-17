struct AA
{
    x: s64 = 10
}

impl AA
{
    func titi(using self, ptr: *AA)
    {
        @assert(ptr.x == 10)
        ptr.x = 666
    }

    func toto(using self)->AA
    {
        var result: retval = create()
        titi(&result)
        return result
    }
}

func create()->AA
{
    var result: retval
    return result
}

#test
{
    var x: AA
    res := x.toto()
    @assert(res.x == 666)
}