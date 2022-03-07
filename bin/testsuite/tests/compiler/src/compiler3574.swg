//#global testerror

struct A
{
}

var G = 0
impl A
{
    mtd(op: string) opAssign(t: func(s32, s32, s32)) {}
    mtd(op: string) opAssign(t: func(s32, s32))
    {
        G = 1
    }
}

#test
{
    var xxxx: A
    xxxx += @(a, b) { }
    @assert(G == 1)
}