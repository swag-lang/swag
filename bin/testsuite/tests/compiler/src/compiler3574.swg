//#global testerror

struct A
{
}

var G = 0
impl A
{
    #[Swag.Overload]
    mtd(op: string) opAssign(t: func(s32, s32, s32))
    {
    }
    #[Swag.Overload]
    mtd(op: string) opAssign(t: func(s32, s32))
    {
        G = 1
    }
}

#test
{
    var xxxx: A
    xxxx += func(a, b)
    {
    }
    @assert(G == 1)
}
