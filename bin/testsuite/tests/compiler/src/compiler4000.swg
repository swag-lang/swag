// YOUHOU 4000 !!!
struct B
{
    x:   s32 = 666
}

var G = 0
impl B
{
    mtd opEquals(v: s32)->bool
    {
        G = 1
        return v == me.x
    }
}

struct A
{
    b:   B
}

impl A
{
    mtd opIndex(i: u64)->const &B
    {
        return &me.b
    }
}

//#[Swag.PrintBc]
#test
{
    var a: A
    @assert(a[0] == 666)
    @assert(G == 1)
}
