//#global skip

struct A
{
    a: s32 = 666
}

impl A
{
    //#[Swag.Macro]
    mtd opIndex(idx: u64)->s32
    {
        @assert(a == 666)
        return 10
    }
}

struct B
{
    d: f32
    using a: A
}

struct C
{
    d: f32
    using b: B
}

//#[Swag.PrintBc]
#test
{
    var v: C
    let x = v[0]
    @assert(x == 10)
}