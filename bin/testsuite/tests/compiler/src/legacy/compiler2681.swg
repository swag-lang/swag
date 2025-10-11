struct AA
{
    x:   s64 = 10
}

impl AA
{
    func titi(me, ptr: *AA)
    {
        @assert(ptr.x == 10)
        ptr.x = 666
    }

    func toto(me)->AA
    {
        var result: retval = create()
        me.titi(&result)
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
    var x:  AA
    let res = x.toto()
    @assert(res.x == 666)
}
