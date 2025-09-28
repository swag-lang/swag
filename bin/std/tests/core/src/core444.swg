using Core

func toto()->Array'u32
{
    var r: retval
    r.add(1)
    r.add(2)
    r.add(3)
    return r
}

//#[Swag.PrintBc]
#test
{
    foreach v in toto()[1 to ]:
        @assert(v > 0)
}
