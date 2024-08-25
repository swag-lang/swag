//#global skip
struct(T) A
{
    x: s32 = 666
}

impl A
{
    /*#[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        @assert(x == 666)
        //#assert T == *s32
        #print T
    }*/

    mtd toto()
    {
    }
}

struct(T) B
{
    using a: A'(*T)
}

struct(T) C
{
    using b: B'(*T)
}

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var v: C's32
    v.toto()
    /*foreach v
    {
    }*/
}