//#global skip
struct(T) A
{
    a: T = 666
}

impl A
{
    #[Swag.Macro]
    mtd(ptr: bool) opVisit(stmt: code)
    {
        @assert(a == 666)
        #mixin stmt
    }
}

struct(T) B
{
    d: f64
    using a: A
}

struct C
{
    x: B's32
}

var G = 0

impl C
{
    //#[Swag.PrintBc]
    mtd toto()
    {
        visit x
        {
            G = 1
        }
    }
}

#test
{
    var v: C
    v.toto()
    @assert(G == 1)
}