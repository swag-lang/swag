struct(T) A
{
    x: s32 = 666
}

impl A
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        @assert(x == 666)
        loop 4
        {
            #mixin stmt
        }

        /*#if @typeof(T).kind == .Struct
        {
            @assert(false)
        }*/
    }
}

struct(T) B
{
    using a: A'(*T)
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v:  B's32
    var cpt = 0
    visit v
    {
        cpt += 1
    }

    @assert(cpt == 4)
}