struct R { a: s32 }
struct C { x: s32 }

func toto(x: R, y: C)
{
    @assert(x.a == 1)
}

#[Swag.Inline]
func from()->C
{
    var r: C
    r.x = 5
    return r
}

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    toto({1}, from())
}
