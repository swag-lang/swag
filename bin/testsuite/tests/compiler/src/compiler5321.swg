//#global skip
struct(R) A
{
    x: s32 = 666
}

impl A
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: code)
    {
        @assert(x == 666)
        #assert R == *s32
    }
}

struct(T) B
{
    using a: A'(*T)
}

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: B's32
    visit v
    {
    }
}