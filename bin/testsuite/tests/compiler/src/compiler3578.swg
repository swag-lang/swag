//#global testerror

struct A
{
}

var G = 0

impl A
{
    mtd(op: string) opAssign(t: closure(s32, s32))
    {
        G = 1
    }

    mtd(op: string) opAssign(t: func(f32, f32)) {}
}

#test
{
    var xxxx: A
    xxxx += closure||(a, b) { }
    @assert(G == 1)
}
