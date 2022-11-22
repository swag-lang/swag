#global skip
// YOUOU 4000 !!!
struct B
{
    x: s32 = 666
}

impl B
{
    mtd opEquals(v: s32)->bool
    {
        @print("XX")
        return v == x
    }
}

struct A
{
    b: B
}

impl A
{
    mtd opIndex(i: uint)->const ref B
    {
        return &b
    }
}

#[Swag.PrintBc]
#test
{
    var a: A
    @assert(a[0] == 666)
}