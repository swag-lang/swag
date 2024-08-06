#global testerror "Err0197"

struct A
{
    a: s32 = 666
}

impl A
{
    #if false
    {
        //#[Swag.Macro]
        mtd opIndex(idx: u64)->s32
        {
            @assert(a == 666)
            return 10
        }
    }
}

struct B
{
    d: f32
    using a: A
}

//#[Swag.PrintBc]
#test
{
    var v: B
    var x = v[0]
    @assert(x == 10)
}