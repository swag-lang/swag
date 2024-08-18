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
    visit v in toto()[1..] do
        @assert(v > 0)
}
