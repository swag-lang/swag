//#global skip
struct A { r, g, b: s32 }

func toto()->A
{
    return {1, 2, 3}
}

impl A
{
    mtd titi()
    {
        with me
        {
            (.r, .g, me.b) = toto()
        }
    }
}

#test
{
    var a: A
    a.titi()
    @assert(a.r == 1)
    @assert(a.g == 2)
    @assert(a.b == 3)
}
