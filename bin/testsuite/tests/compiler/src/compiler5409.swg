//#global skip

struct A
{
    x:   s32
}

var g = 666

func toto()->&s32
{
    return &g
}

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
#test
{
    var a: A{toto()}
    @assert(a.x == 666)
}
