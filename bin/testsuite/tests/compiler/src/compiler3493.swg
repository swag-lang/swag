//#global skip

struct A
{
    a:   s32 = 666
}

impl A
{
    //#[Swag.Macro]
    mtd opIndex(idx: u64)->s32
    {
        @assert(me.a == 666)
        return 10
    }
}

struct B
{
    using a: A
}

//#[Swag.PrintBc]
#test
{
    var v: B
    let x  = v[0]
    @assert(x == 10)
}
