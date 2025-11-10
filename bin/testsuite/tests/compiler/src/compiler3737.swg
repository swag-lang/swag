struct A
{
    x:   s32
}

interface I
{
    func x(me);
}

impl I for A
{
    mtd impl x() {}
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var v: *A
    let x  = cast(I) cast(*A) v
    let t  = @kindof(x)
    @assert(t and @countof(t.fullname))
}
