struct(T) A
{
    x:   s32 = 666
}

impl A
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        @assert(me.x == 666)
        for 4
        {
            #inject(stmt)
        }

        /*#if #typeof(T).kind == .Struct
        {
            @assert(false)
        }*/
    }
}

struct(T) B
{
    using a: A'(*T)
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var v:  B's32
    var cpt = 0
    foreach v
    {
        cpt += 1
    }

    @assert(cpt == 4)
}
