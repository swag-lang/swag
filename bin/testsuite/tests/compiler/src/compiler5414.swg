//#global skip
struct A
{
    x    = 666
}

impl A
{
    mtd length()->s32
    {
        return me.x
    }

    mtd opIndex(x: u32)->&*A
    {
        return &g1
    }
}

var g:  A
var g1: *A

func titi()->&*A
{
    return &g1
}

//#[Swag.PrintBc]
#test
{
    g1 = &g
    let a = g[0].length()
    @assert(a == 666)
}
